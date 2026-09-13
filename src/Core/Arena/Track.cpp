#include <Core/Arena/Track.hpp>

#include <BLIB/AI/PathFinder.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Arena/Terrain.hpp>

namespace core
{
namespace arena
{
namespace
{
struct NodeHasher {
    std::size_t operator()(const Node& node) const {
        return bl::util::hashCombine(node.index.x, node.index.y);
    }
};

using PathFinder = bl::ai::PathFinder<Node, NodeHasher>;

constexpr float HeightWeight        = 10.f;
constexpr float MaxHeightDiffNormal = 0.05f;
constexpr int DistanceCost          = 10;
constexpr int DiagonalCost          = static_cast<int>(static_cast<float>(DistanceCost) * 1.4142f);

int nodeMovementCost(const Node& from, const Node& to) {
    const float heightDiff = std::abs(from.height - to.height);
    const bool diagonal    = (from.index.x != to.index.x && from.index.y != to.index.y);
    return static_cast<int>(heightDiff * HeightWeight) + (diagonal ? 14 : 10);
}

float getInterpolationFactor(const TrackNode& node, float globalDistance) {
    globalDistance -= node.accumulatedDistance;
    return glm::clamp(globalDistance / node.length, 0.f, 1.f);
}

} // namespace

Track::Track() {}

void Track::generate(const Terrain& terrain) {
    const float maxHeightDiff = MaxHeightDiffNormal * terrain.getMaxHeight();

    const auto yieldAdjacentNodes =
        [&terrain, maxHeightDiff](const Node& node, std::vector<std::pair<Node, int>>& adjacent) {
            const glm::u32vec2 idx = node.index;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;

                    glm::i32vec2 neighborIdx(idx.x + dx, idx.y + dy);
                    if (neighborIdx.x < 0 || neighborIdx.y < 0 ||
                        neighborIdx.x >= terrain.getNodes().getWidth() ||
                        neighborIdx.y >= terrain.getNodes().getHeight()) {
                        continue;
                    }

                    const Node& neighbor = terrain.getNodes()(neighborIdx.x, neighborIdx.y);

                    // TODO - adjust cost and do bridges or tunnels
                    if (std::abs(neighbor.height - node.height) >= maxHeightDiff) { continue; }

                    const int cost = nodeMovementCost(node, neighbor);
                    adjacent.emplace_back(neighbor, cost);
                }
            }
        };

    // TODO - better start and end nodes
    const unsigned int middleY = terrain.getNodes().getHeight() / 2;
    const Node& start          = terrain.getNodes()(0, middleY);
    const Node& end            = terrain.getNodes()(terrain.getNodes().getWidth() - 1, middleY);

    std::vector<Node> path;
    if (!PathFinder::findPath(start, end, yieldAdjacentNodes, &nodeMovementCost, path)) {
        BL_LOG_ERROR << "Failed to find a path for the track";
        return;
    }

    float accumulatedDistance = 0.f;
    nodes.reserve(path.size());
    for (const Node& terrainNode : path) {
        auto& node = nodes.emplace_back();
        node.position =
            glm::vec3(terrainNode.worldPos.x, terrainNode.height, terrainNode.worldPos.y);

        glm::vec3 prevNodePos = node.position - glm::vec3(-1.f, 0.f, 0.f);
        if (nodes.size() > 1) {
            auto& prev  = nodes[nodes.size() - 2];
            prevNodePos = prev.position;
        }

        glm::vec3 nextNodePos     = node.position + glm::vec3(1.f, 0.f, 0.f);
        glm::vec3 nextNextNodePos = nextNodePos + glm::vec3(1.f, 0.f, 0.f);
        if (nodes.size() < path.size()) {
            const auto& next = path[nodes.size()];
            nextNodePos      = glm::vec3(next.worldPos.x, next.height, next.worldPos.y);
        }
        if (nodes.size() < path.size() - 1) {
            const auto& nextNext = path[nodes.size() + 1];
            nextNextNodePos = glm::vec3(nextNext.worldPos.x, nextNext.height, nextNext.worldPos.y);
        }

        node.length              = glm::distance(node.position, nextNodePos);
        node.accumulatedDistance = accumulatedDistance;
        accumulatedDistance += node.length;
        node.spline.init(prevNodePos, node.position, nextNodePos, nextNextNodePos);
    }
}

float Track::getTrackLength() const {
    return nodes.empty() ? 0.f : nodes.back().accumulatedDistance;
}

glm::vec3 Track::getPositionAtDistance(float distance) const {
    const TrackNode& node = getNodeAtDistance(distance);
    const float t         = getInterpolationFactor(node, distance);
    return node.spline.evaluate(t);
}

glm::vec3 Track::getDirectionAtDistance(float distance) const {
    const TrackNode& node = getNodeAtDistance(distance);
    const float t         = getInterpolationFactor(node, distance);
    return glm::normalize(node.spline.derivative(t));
}

glm::vec3 Track::getRightAtDistance(float distance) const {
    const glm::vec3 dir = getDirectionAtDistance(distance);
    return glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f)));
}

glm::vec3 Track::getUpAtDistance(float distance) const {
    const TrackNode& node = getNodeAtDistance(distance);
    const float t         = getInterpolationFactor(node, distance);
    const glm::vec3 dir   = glm::normalize(node.spline.derivative(t));
    const glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f)));
    return glm::normalize(glm::cross(right, dir));
}

const TrackNode& Track::getNodeAtDistance(float d) const {
    // binary search to find node where accumulatedDistance <= d < nextNode.accumulatedDistance
    const auto it =
        std::lower_bound(nodes.begin(), nodes.end(), d, [](const TrackNode& node, float distance) {
            return node.accumulatedDistance < distance;
        });
    if (it == nodes.end()) { return nodes.back(); }
    return *it;
}

} // namespace arena
} // namespace core

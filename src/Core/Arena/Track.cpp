#include <Core/Arena/Train/Track.hpp>

#include <BLIB/AI/PathFinder.hpp>
#include <BLIB/Logging.hpp>
#include <Core/Arena/Terrain.hpp>

namespace core
{
namespace arena
{
namespace train
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

constexpr float TrackHeight                = 0.25f;
constexpr float TrackWidth                 = 1.5f;
constexpr float TrackHalfWidth             = TrackWidth * 0.5f;
constexpr float RailWidth                  = 0.08f;
constexpr float RailHalfWidth              = RailWidth * 0.5f;
constexpr float RailHeight                 = 0.05f;
constexpr float RailHalfHeight             = RailHeight * 0.5f;
constexpr unsigned int RailNumSides        = 4;
constexpr unsigned int RailVerticesPerStep = 8 * 2;
constexpr unsigned int RailIndicesPerStep  = 24 * 2;
constexpr float TieLength                  = 0.2f;
constexpr float TieHalfLength              = TieLength * 0.5f;
constexpr float TieWidth                   = TrackWidth * 1.2f;
constexpr float TieHalfWidth               = TieWidth * 0.5f;
constexpr float TieSpacing                 = 1.f;
constexpr float TieHeight                  = 0.1f;
constexpr float TieHalfHeight              = TieHeight * 0.5f;
constexpr unsigned int TieVertices         = 8;
constexpr unsigned int TieIndices          = 36;

constexpr unsigned int TieFaces[6][4] = {
    {0, 2, 6, 4}, // +dir
    {1, 5, 7, 3}, // -dir
    {0, 4, 5, 1}, // +up
    {2, 3, 7, 6}, // -up
    {0, 1, 3, 2}, // +right
    {4, 6, 7, 5}, // -right
};

int nodeMovementCost(const Node& from, const Node& to) {
    const float heightDiff = std::abs(from.height - to.height);
    const bool diagonal    = (from.index.x != to.index.x && from.index.y != to.index.y);
    return static_cast<int>(heightDiff * HeightWeight) + (diagonal ? 14 : 10);
}

float getInterpolationFactor(const TrackNode& node, float globalDistance) {
    globalDistance -= node.accumulatedDistance;
    return glm::clamp(globalDistance / node.length, 0.f, 1.f);
}

unsigned int calculateRailSliceCount(float length, float step) {
    return static_cast<unsigned int>(std::ceil(length / step)) + 1;
}

unsigned int calculateTieCount(float length, float spacing) {
    return static_cast<unsigned int>(length / spacing);
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
    nodes.clear();
    nodes.reserve(path.size());
    for (const Node& terrainNode : path) {
        auto& node    = nodes.emplace_back();
        node.position = glm::vec3(terrainNode.worldPos.x,
                                  terrain.sampleHeight(terrainNode.worldPos),
                                  terrainNode.worldPos.y);

        glm::vec3 prevNodePos = node.position - glm::vec3(-1.f, 0.f, 0.f);
        if (nodes.size() > 1) {
            auto& prev  = nodes[nodes.size() - 2];
            prevNodePos = prev.position;
        }

        glm::vec3 nextNodePos     = node.position + glm::vec3(1.f, 0.f, 0.f);
        glm::vec3 nextNextNodePos = nextNodePos + glm::vec3(1.f, 0.f, 0.f);
        if (nodes.size() < path.size()) {
            const auto& next = path[nodes.size()];
            nextNodePos =
                glm::vec3(next.worldPos.x, terrain.sampleHeight(next.worldPos), next.worldPos.y);
        }
        if (nodes.size() < path.size() - 1) {
            const auto& nextNext = path[nodes.size() + 1];
            nextNextNodePos      = glm::vec3(
                nextNext.worldPos.x, terrain.sampleHeight(nextNext.worldPos), nextNext.worldPos.y);
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
    if (nodes.empty()) { return glm::vec3(0.f); }

    const TrackNode& node = getNodeAtDistance(distance);
    const float t         = getInterpolationFactor(node, distance);
    glm::vec3 pos         = node.spline.evaluate(t);
    pos.y += TrackHeight;
    return pos;
}

glm::vec3 Track::getDirectionAtDistance(float distance) const {
    if (nodes.empty()) { return glm::vec3(0.f, 0.f, 1.f); }

    const TrackNode& node = getNodeAtDistance(distance);
    const float t         = getInterpolationFactor(node, distance);
    return glm::normalize(node.spline.derivative(t));
}

glm::vec3 Track::getRightAtDistance(float distance) const {
    if (nodes.empty()) { return glm::vec3(1.f, 0.f, 0.f); }

    const glm::vec3 dir = getDirectionAtDistance(distance);
    return glm::normalize(glm::cross(dir, glm::vec3(0.f, 1.f, 0.f)));
}

glm::vec3 Track::getUpAtDistance(float distance) const {
    if (nodes.empty()) { return glm::vec3(0.f, 1.f, 0.f); }

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

    if (it->accumulatedDistance > d && it != nodes.begin()) { return *std::prev(it); }

    return *it;
}

float Track::findDistanceFromOffset(float startDistance, float offset, float threshold) const {
    // TODO - consider a more robust search method
    (void)threshold;
    return startDistance + offset;
}

void Track::addToWorld(bl::engine::World& world, float s) {
    step = s;

    const unsigned int steps = calculateRailSliceCount(getTrackLength(), step);
    railsDrawable.create(world, steps * RailVerticesPerStep, (steps - 1) * RailIndicesPerStep);

    const unsigned int tieCount = calculateTieCount(getTrackLength(), TieSpacing + TieLength);
    tiesDrawable.create(world, tieCount * TieVertices, tieCount * TieIndices);

    generateGeometry();

    railsDrawable.addToScene(world.scene(), bl::rc::UpdateSpeed::Static);
    tiesDrawable.addToScene(world.scene(), bl::rc::UpdateSpeed::Static);
}

void Track::generateGeometry() {
    if (nodes.empty()) { return; }
    if (!railsDrawable.exists()) { return; }

    std::uint32_t railVertexOffset = 0;
    std::uint32_t railIndexOffset  = 0;
    generateRail(-TrackHalfWidth, railVertexOffset, railIndexOffset);
    generateRail(TrackHalfWidth, railVertexOffset, railIndexOffset);
    generateTies();
}

void Track::generateRail(float offset, std::uint32_t& vertexOffset, std::uint32_t& indexOffset) {
    const unsigned int numSlices = calculateRailSliceCount(getTrackLength(), step);
    const float stepPerSlice     = getTrackLength() / static_cast<float>(numSlices - 1);
    railsDrawable.resize(numSlices * RailVerticesPerStep, (numSlices - 1) * RailIndicesPerStep);

    const auto connectSlice = [this, &vertexOffset, &indexOffset]() {
        const unsigned int base = vertexOffset - 8;

        // top
        railsDrawable.getIndex(indexOffset++) = base + 0;
        railsDrawable.getIndex(indexOffset++) = base + 4;
        railsDrawable.getIndex(indexOffset++) = base + 6;
        railsDrawable.getIndex(indexOffset++) = base + 0;
        railsDrawable.getIndex(indexOffset++) = base + 6;
        railsDrawable.getIndex(indexOffset++) = base + 2;

        // bottom
        railsDrawable.getIndex(indexOffset++) = base + 1;
        railsDrawable.getIndex(indexOffset++) = base + 3;
        railsDrawable.getIndex(indexOffset++) = base + 7;
        railsDrawable.getIndex(indexOffset++) = base + 1;
        railsDrawable.getIndex(indexOffset++) = base + 7;
        railsDrawable.getIndex(indexOffset++) = base + 5;

        // +right
        railsDrawable.getIndex(indexOffset++) = base + 0;
        railsDrawable.getIndex(indexOffset++) = base + 1;
        railsDrawable.getIndex(indexOffset++) = base + 5;
        railsDrawable.getIndex(indexOffset++) = base + 0;
        railsDrawable.getIndex(indexOffset++) = base + 5;
        railsDrawable.getIndex(indexOffset++) = base + 4;

        // -right
        railsDrawable.getIndex(indexOffset++) = base + 2;
        railsDrawable.getIndex(indexOffset++) = base + 6;
        railsDrawable.getIndex(indexOffset++) = base + 7;
        railsDrawable.getIndex(indexOffset++) = base + 2;
        railsDrawable.getIndex(indexOffset++) = base + 7;
        railsDrawable.getIndex(indexOffset++) = base + 3;
    };

    const auto setVertex = [this, &vertexOffset](const glm::vec3& pos) {
        auto& v = railsDrawable.getVertex(vertexOffset++);
        v.pos   = pos;
        v.color = glm::vec4(0.7f, 0.7f, 0.7f, 1.f);
    };

    for (unsigned int i = 0; i < numSlices; ++i) {
        const float distance       = static_cast<float>(i) * stepPerSlice;
        const glm::vec3 pos        = getPositionAtDistance(distance);
        const glm::vec3 up         = getUpAtDistance(distance);
        const glm::vec3 right      = getRightAtDistance(distance);
        const glm::vec3 railCenter = pos + right * offset;

        setVertex(railCenter + right * RailHalfWidth + up * RailHalfHeight);
        setVertex(railCenter + right * RailHalfWidth - up * RailHalfHeight);
        setVertex(railCenter - right * RailHalfWidth + up * RailHalfHeight);
        setVertex(railCenter - right * RailHalfWidth - up * RailHalfHeight);

        if (i > 0) { connectSlice(); }
    }

    railsDrawable.commit();
}

void Track::generateTies() {
    const unsigned int tieCount = calculateTieCount(getTrackLength(), TieSpacing + TieLength);
    tiesDrawable.resize(tieCount * TieVertices, tieCount * TieIndices);

    std::uint32_t vertexOffset = 0;
    std::uint32_t indexOffset  = 0;

    const auto emitVertex = [this, &vertexOffset](const glm::vec3& pos) {
        auto& v = tiesDrawable.getVertex(vertexOffset++);
        v.pos   = pos;
        v.color = glm::vec4(0.5f, 0.3f, 0.2f, 1.f);
    };

    for (unsigned int i = 0; i < tieCount; ++i) {
        const float distance  = static_cast<float>(i) * (TieSpacing + TieLength);
        const glm::vec3 pos   = getPositionAtDistance(distance);
        const glm::vec3 dir   = getDirectionAtDistance(distance);
        const glm::vec3 up    = getUpAtDistance(distance);
        const glm::vec3 right = getRightAtDistance(distance);

        emitVertex(pos + right * TieHalfWidth + up * TieHalfHeight + dir * TieHalfLength);
        emitVertex(pos + right * TieHalfWidth + up * TieHalfHeight - dir * TieHalfLength);
        emitVertex(pos + right * TieHalfWidth - up * TieHalfHeight + dir * TieHalfLength);
        emitVertex(pos + right * TieHalfWidth - up * TieHalfHeight - dir * TieHalfLength);
        emitVertex(pos - right * TieHalfWidth + up * TieHalfHeight + dir * TieHalfLength);
        emitVertex(pos - right * TieHalfWidth + up * TieHalfHeight - dir * TieHalfLength);
        emitVertex(pos - right * TieHalfWidth - up * TieHalfHeight + dir * TieHalfLength);
        emitVertex(pos - right * TieHalfWidth - up * TieHalfHeight - dir * TieHalfLength);

        const unsigned int baseIndex = vertexOffset - TieVertices;
        for (const auto& face : TieFaces) {
            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[0];
            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[1];
            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[2];

            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[0];
            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[2];
            tiesDrawable.getIndex(indexOffset++) = baseIndex + face[3];
        }
    }

    tiesDrawable.commit();
}

} // namespace train
} // namespace arena
} // namespace core

#include <Core/World/World.hpp>

#include <BLIB/AI/PathFinder.hpp>
#include <BLIB/Math.hpp>
#include <Core/Components/WorldNode.hpp>
#include <Core/Game.hpp>
#include <Core/World/Collisions.hpp>

namespace core
{
namespace world
{
namespace
{
using PathFinder = bl::ai::PathFinder<Path::Waypoint, Path::WaypointHash>;

const bl::rc::Color NodeFreeColor     = sf::Color(20, 230, 65);
const bl::rc::Color NodeOccupiedColor = sf::Color(230, 65, 20);

struct UnitQueryContext {
    bl::ecs::Registry* ecs;
    std::vector<com::Unit*>* result;
    bl::ecs::Transaction<bl::ecs::tx::EntityUnlocked, bl::ecs::tx::ComponentRead<com::Unit>>* tx;
};

bool unitQueryCallback(bl::com::Physics2D* physics, void* ctx) {
    UnitQueryContext* queryCtx = static_cast<UnitQueryContext*>(ctx);
    com::Unit* unit = queryCtx->ecs->getComponent<com::Unit>(physics->getOwner(), *queryCtx->tx);
    if (unit) { queryCtx->result->emplace_back(unit); }
    return true;
}

} // namespace

World::World(bl::engine::Engine& e)
: World2D(e) {}

void World::addNode(Node::Type type, glm::vec2 pos) {
    Game& game = Game::getInstance<Game>();

    if (!debugNodes.exists()) {
        debugNodes.create(*this, 1024);
        debugNodes.resize(0, false);
        debugNodes.getTransform().setDepth(1.f);
        debugNodes.addToSceneWithCustomPipeline(
            scene(), bl::rc::UpdateSpeed::Static, bl::rc::Config::PipelineIds::Unlit2DGeometry);
    }
    if (!debugNodeEdges.exists()) {
        debugNodeEdges.create(*this, 128);
        debugNodeEdges.resize(0, false);
        debugNodeEdges.addToSceneWithCustomPipeline(
            scene(), bl::rc::UpdateSpeed::Static, sys::Render::NodeEdgesPipelineId);
    }

    auto& node = nodes.emplace_back(type, pos);
    computeNodeDistances(node);
    populateNodeEdges(node);
    addDebugGraphicsToNode(node);

    const float radius = 1.f / getWorldToBoxScale();
    auto entity        = createEntity();
    auto* transform    = engine().ecs().emplaceComponent<bl::com::Transform2D>(entity, pos);
    engine().ecs().emplaceComponent<bl::com::Hitbox2D>(entity, transform, radius);
    game.renderSystem().addTestGraphicsToEntity(entity, radius, NodeFreeColor);
    game.physicsSystem().createSensorForEntity(entity, Collisions::getSensorFilter());
    node.sensorEntity = entity;
    engine().ecs().emplaceComponent<com::WorldNode>(entity, this, nodes.size() - 1);
}

void World::addCover(glm::vec2 pos, glm::vec2 size, float angle) {
    Game& game = Game::getInstance<Game>();

    auto entity = createEntity();
    covers.emplace_back(entity, pos, size, angle);
    auto* transform = engine().ecs().emplaceComponent<bl::com::Transform2D>(entity, pos, angle);
    game.renderSystem().addTestGraphicsToEntity(entity, size, sf::Color::Black);

    auto bodyDef      = b2DefaultBodyDef();
    auto shapeDef     = b2DefaultShapeDef();
    bodyDef.type      = b2_staticBody;
    shapeDef.filter   = Collisions::getCoverFilter();
    shapeDef.friction = 0.f;
    engine().ecs().emplaceComponent<bl::com::Hitbox2D>(entity, transform, size);
    game.physicsSystem().addPhysicsToEntity(entity, bodyDef, shapeDef);

    recomputeNodeDistances();
    repopulateAllNodeEdges();
}

bool World::removeNodeAtPosition(const glm::vec2& pos, float thresh) {
    Node* node = nullptr;
    float md   = 0.f;
    for (Node& n : nodes) {
        const float d = glm::distance(n.position, pos);
        if (d <= thresh) {
            if (!node || d < md) {
                node = &n;
                md   = d;
            }
        }
    }

    if (node) {
        Game& game = Game::getInstance<Game>();
        game.engine().ecs().destroyEntity(node->sensorEntity);
        const auto i = node - nodes.data();
        nodes.erase(nodes.begin() + i);
        regenNodeGraphics();
        repopulateAllNodeEdges();
        return true;
    }
    return false;
}

bool World::removeCoverAtPosition(const glm::vec2& pos) {
    Game& game = Game::getInstance<Game>();
    bl::com::Physics2D* phys =
        game.physicsSystem().findEntityAtPosition(*this, pos, Collisions::getCoverQueryFilter());
    if (phys) {
        for (auto it = covers.begin(); it != covers.end(); ++it) {
            if (it->entity == phys->getOwner()) {
                covers.erase(it);
                break;
            }
        }
        game.engine().ecs().destroyEntity(phys->getOwner());
        return true;
    }
    return false;
}

void World::handleSensorEnter(std::size_t i, bl::ecs::Entity entity) {
    auto& node = nodes[i];
    if (node.occupiedBy == bl::ecs::InvalidEntity) {
        Game& game = Game::getInstance<Game>();
        game.renderSystem().updateTestGraphicsColor(node.sensorEntity, NodeOccupiedColor);

        node.occupiedBy = entity;
    }
    else {
        BL_LOG_WARN
            << "Node entered by second entity. Make an actual heuristic to decide occupation";
    }
}

void World::handleSensorExit(std::size_t i, bl::ecs::Entity entity) {
    auto& node = nodes[i];
    if (node.occupiedBy == entity) {
        Game& game = Game::getInstance<Game>();
        game.renderSystem().updateTestGraphicsColor(node.sensorEntity, NodeFreeColor);

        node.occupiedBy = bl::ecs::InvalidEntity;
    }
    else {
        BL_LOG_WARN
            << "Node exited by different entity. Make an actual heuristic to decide occupation";
    }
}

void World::recomputeNodeDistances() {
    if (!nodes.empty()) {
        debugNodes.resize(0, false);
        for (auto& node : nodes) {
            computeNodeDistances(node);
            addDebugGraphicsToNode(node);
        }
    }
}

void World::computeNodeDistances(Node& node) {
    const float MaxCoverDistance = Node::MaxCoverDistance * getWorldToBoxScale();
    unsigned int i               = 0;
    for (int angle = 0; angle < 360; angle += 30, ++i) {
        const float r = bl::math::degreesToRadians(static_cast<float>(angle));
        const float c = std::cos(r);
        const float s = std::sin(r);
        // TODO - consider casting a circle?
        const auto result = b2World_CastRayClosest(
            getBox2dWorldId(),
            {node.position.x * getWorldToBoxScale(), node.position.y * getWorldToBoxScale()},
            {c * MaxCoverDistance, s * MaxCoverDistance},
            Collisions::getCoverQueryFilter());
        if (result.hit) {
            node.distanceToCover[i] = glm::distance(
                glm::vec2(result.point.x, result.point.y) / getWorldToBoxScale(), node.position);
        }
        else { node.distanceToCover[i] = Node::MaxCoverDistance; }
    }
}

void World::populateNodeEdges(Node& node) {
    const b2Vec2 nodePos{node.position.x * getWorldToBoxScale(),
                         node.position.y * getWorldToBoxScale()};

    for (auto& otherNode : nodes) {
        if (&otherNode == &node) { continue; }

        const glm::vec2 otherNodePos = otherNode.position * getWorldToBoxScale();
        const auto cast =
            b2World_CastRayClosest(getBox2dWorldId(),
                                   nodePos,
                                   {otherNodePos.x - nodePos.x, otherNodePos.y - nodePos.y},
                                   Collisions::getNodeConnectionFilter());
        if (cast.hit) {
            if (bl::sys::Physics2D::getEntityFromShape(cast.shapeId) == otherNode.sensorEntity) {
                node.connectsTo.emplace_back(&otherNode - nodes.data());
                otherNode.connectsTo.emplace_back(&node - nodes.data());

                // debug gfx
                debugNodeEdges.resize(debugNodeEdges.getSize() + 2);
                debugNodeEdges[debugNodeEdges.getSize() - 2].pos   = glm::vec3(node.position, 0.f);
                debugNodeEdges[debugNodeEdges.getSize() - 2].color = {0.25f, 0.05f, 0.05f, 1.f};
                debugNodeEdges[debugNodeEdges.getSize() - 1].pos =
                    glm::vec3(otherNode.position, 0.f);
                debugNodeEdges[debugNodeEdges.getSize() - 1].color = {0.25f, 0.05f, 0.05f, 1.f};
            }
        }
    }
}

void World::repopulateAllNodeEdges() {
    if (!nodes.empty()) {
        for (auto& node : nodes) { node.connectsTo.clear(); }
        debugNodeEdges.resize(0, false);
        for (auto& node : nodes) { populateNodeEdges(node); }
    }
}

void World::addDebugGraphicsToNode(Node& node) {
    constexpr unsigned int CircleTriangleCount = 100;
    constexpr unsigned int CircleVertexCount   = CircleTriangleCount * 3;
    constexpr unsigned int ArrowCount          = 12;
    constexpr unsigned int ArrowTriangleCount  = 1;
    constexpr unsigned int ArrowVertexCount    = ArrowCount * ArrowTriangleCount * 3;
    constexpr unsigned int VertexCount         = CircleVertexCount + ArrowVertexCount;

    constexpr float Radius     = 10.f;
    constexpr float ArrowWidth = 6.f;

    const glm::vec3 centerPos(node.position, 0.f);
    const unsigned int bi = debugNodes.getSize();
    debugNodes.resize(debugNodes.getSize() + VertexCount);

    // create circle
    const bl::rc::Color circleColor =
        node.getType() == world::Node::Path ? sf::Color::Green : sf::Color::Red;
    for (unsigned int ci = 0; ci < CircleTriangleCount; ++ci) {
        const unsigned int b = ci * 3;

        auto& center = debugNodes[bi + b];
        center.color = circleColor;
        center.pos   = centerPos;

        for (unsigned int j = 1; j < 3; ++j) {
            const unsigned int i = b + j;
            auto& v              = debugNodes[bi + i];
            const float d =
                static_cast<float>(ci + j - 1) / static_cast<float>(CircleTriangleCount) * 360.f;
            const float r = bl::math::degreesToRadians(d);
            v.color       = circleColor;
            v.pos.x       = std::cos(r) * Radius + centerPos.x;
            v.pos.y       = std::sin(r) * Radius + centerPos.y;
            v.pos.z       = 0.f;
        }
    }

    // render triangles
    for (unsigned int ai = 0; ai < ArrowCount; ++ai) {
        const unsigned int b = ai * ArrowTriangleCount * 3 + CircleVertexCount;
        const float degrees  = static_cast<float>(ai) * 30.f;
        const float d        = node.getDistanceToCover(degrees);
        const float f        = d / world::Node::MaxCoverDistance;
        const float r        = bl::math::degreesToRadians(degrees);
        const float c        = std::cos(r);
        const float s        = std::sin(r);

        const bl::rc::Color arrowColor = node.getType() == world::Node::Path ?
                                             sf::Color::Transparent :
                                             sf::Color(255.f * f, 255.f * (1.f - f), 20, 120);

        auto& p0 = debugNodes[bi + b];
        p0.color = arrowColor;
        p0.pos.x = d * c + centerPos.x;
        p0.pos.y = d * s + centerPos.y;
        p0.pos.z = 0.f;

        auto& p1 = debugNodes[bi + b + 1];
        p1.color = arrowColor;
        p1.pos.x = ArrowWidth * -s + centerPos.x;
        p1.pos.y = ArrowWidth * c + centerPos.y;
        p1.pos.z = 0.f;

        auto& p2 = debugNodes[bi + b + 2];
        p2.color = arrowColor;
        p2.pos.x = ArrowWidth * s + centerPos.x;
        p2.pos.y = ArrowWidth * -c + centerPos.y;
        p2.pos.z = 0.f;
    }
}

void World::regenNodeGraphics() {
    debugNodes.resize(0, false);
    for (auto& node : nodes) { addDebugGraphicsToNode(node); }
}

const Node* World::getClosestReachableNode(const glm::vec2& position,
                                           const glm::vec2& dstPos) const {
    const Node* closest   = nullptr;
    float closestDistance = std::numeric_limits<float>::max();
    for (const Node& node : nodes) { // TODO - spatial partition
        const float toNode = glm::distance(node.getPosition(), position);
        const float toDst  = glm::distance(node.getPosition(), dstPos);
        const float total  = toNode + toDst;
        if (total < closestDistance || !closest) {
            if (node.getOccupier() == bl::ecs::InvalidEntity) {
                if (pathToNodeIsClear(position, node)) {
                    closest         = &node;
                    closestDistance = total;
                }
            }
        }
    }
    return closest;
}

bool World::computePath(const glm::vec2& startPos, const glm::vec2& dst, Path& path) {
    using Waypoint = Path::Waypoint;

    // perform path finding
    const auto distanceCb = [](const Waypoint& a, const Waypoint& b) -> int {
        return static_cast<int>(glm::distance(a.position, b.position));
    };
    const auto adjacentNodeCb = [this, &dst](const Waypoint& wp,
                                             std::vector<std::pair<Waypoint, int>>& neighbors) {
        if (wp.worldNode) {
            neighbors.reserve(wp.worldNode->connectsTo.size() + 1);
            for (std::uint32_t i : wp.worldNode->connectsTo) {
                const Node* pn = &nodes[i];
                if (pn->getOccupier() == bl::ecs::InvalidEntity) {
                    neighbors.emplace_back(
                        Waypoint(pn->position, pn),
                        static_cast<int>(glm::distance(wp.position, pn->position)));
                }
            }
        }
        else {
            neighbors.reserve(16);
            for (const Node& node : nodes) {
                const float d = glm::distance(wp.position, node.position);
                if (pathToNodeIsClear(wp.position, node)) {
                    neighbors.emplace_back(Waypoint(node.position, &node), static_cast<int>(d));
                }
            }
        }

        if (pathToPositionIsClear(wp.position, dst)) {
            neighbors.emplace_back(Waypoint(dst),
                                   static_cast<int>(glm::distance(wp.position, dst)));
        }
    };

    return PathFinder::findPath({startPos, getNodeAtPosition(startPos)},
                                {dst, getNodeAtPosition(dst)},
                                adjacentNodeCb,
                                distanceCb,
                                path.waypoints);
}

bool World::pathToPositionIsClear(const glm::vec2& start, const glm::vec2& end) const {
    const float s        = getWorldToBoxScale();
    const glm::vec2 diff = (end - start) * s;
    const auto result    = b2World_CastRayClosest(getBox2dWorldId(),
                                                  {start.x * s, start.y * s},
                                                  {diff.x, diff.y},
                                               Collisions::getUnitMovementQueryFilter());
    return !result.hit;
}

bool World::pathToNodeIsClear(const glm::vec2& pos, const Node& node) const {
    return pathToPositionIsClear(pos, node.position);
}

com::Target* World::getTargetAtPosition(const glm::vec2& pos) const {
    auto& game = bl::game::Game::getInstance<Game>();
    auto* phys =
        game.physicsSystem().findEntityAtPosition(*this, pos, Collisions::getTargetQueryFilter());
    return phys != nullptr ? engine().ecs().getComponent<com::Target>(phys->getOwner()) : nullptr;
}

com::Unit* World::getUnitAtPosition(const glm::vec2& pos) const {
    auto& game = bl::game::Game::getInstance<Game>();
    auto* phys =
        game.physicsSystem().findEntityAtPosition(*this, pos, Collisions::getUnitQueryFilter());
    return phys != nullptr ? engine().ecs().getComponent<com::Unit>(phys->getOwner()) : nullptr;
}

const Node* World::getNodeAtPosition(const glm::vec2& pos, float thresh) const {
    const Node* closest = nullptr;
    float cdist         = 0.f;
    for (const Node& node : nodes) {
        const float d = glm::distance(node.position, pos);
        if (d <= thresh) {
            if (d < cdist || !closest) {
                cdist   = d;
                closest = &node;
            }
        }
    }
    return closest;
}

std::vector<com::Unit*> World::getUnitsInArea(const sf::FloatRect& region) const {
    std::vector<com::Unit*> result;
    result.reserve(64);

    bl::ecs::Transaction<bl::ecs::tx::EntityUnlocked, bl::ecs::tx::ComponentRead<com::Unit>> tx(
        engine().ecs());
    UnitQueryContext ctx{&engine().ecs(), &result, &tx};
    queryAABB({region.left, region.top},
              {region.left + region.width, region.top + region.height},
              Collisions::getUnitQueryFilter(),
              &unitQueryCallback,
              &ctx);

    return result;
}

} // namespace world
} // namespace core

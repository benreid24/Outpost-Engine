#include <Core/World/World.hpp>

#include <BLIB/Math.hpp>
#include <Core/Game.hpp>
#include <Core/World/Collisions.hpp>

namespace core
{
namespace world
{
World::World(bl::engine::Engine& e)
: World2D(e) {}

void World::addNode(Node::Type type, glm::vec2 pos) {
    Game& game = Game::getInstance<Game>();
    auto& node = nodes.emplace_back(type, pos);
    computeNodeDistances(node);
    game.renderSystem().addDebugGraphicsToNode(node);
}

void World::addCover(glm::vec2 pos, glm::vec2 size, float angle) {
    Game& game = Game::getInstance<Game>();
    covers.emplace_back(pos, size, angle);

    auto entity     = createEntity();
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
}

void World::recomputeNodeDistances() {
    Game& game = Game::getInstance<Game>();
    for (auto& node : nodes) {
        computeNodeDistances(node);
        game.renderSystem().addDebugGraphicsToNode(node);
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

} // namespace world
} // namespace core

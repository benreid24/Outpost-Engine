#include <Core/World/World.hpp>

#include <BLIB/Math.hpp>
#include <Core/World/Collisions.hpp>

namespace core
{
namespace world
{
World::World(bl::engine::Engine& e)
: World2D(e) {}

void World::addNode(Node::Type type, glm::vec2 pos) {
    auto& node = nodes.emplace_back(type, pos);

    const float MaxCoverDistance = Node::MaxCoverDistance * getWorldToBoxScale();
    unsigned int i               = 0;
    for (int angle = 0; angle < 360; angle += 30, ++i) {
        const float r = bl::math::degreesToRadians(static_cast<float>(angle));
        const float c = std::cos(r);
        const float s = std::cos(r);
        // TODO - consider casting a circle?
        const auto result =
            b2World_CastRayClosest(getBox2dWorldId(),
                                   {pos.x * getWorldToBoxScale(), pos.y * getWorldToBoxScale()},
                                   {c * MaxCoverDistance, s * MaxCoverDistance},
                                   Collisions::getCoverQueryFilter());
        if (result.hit) {
            node.distanceToCover[i] = glm::distance(
                glm::vec2(result.point.x, result.point.y) / getWorldToBoxScale(), pos);
        }
        else { node.distanceToCover[i] = Node::MaxCoverDistance; }
    }

    // TODO - add test graphics
}

void World::addCover(glm::vec2 pos, glm::vec2 size, float angle) {
    covers.emplace_back(pos, size, angle);
    // TODO - add test graphics & physics body
    // TODO - think about collision filters
}

} // namespace world
} // namespace core

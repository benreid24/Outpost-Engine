#include <Core/World/Node.hpp>

#include <cmath>

namespace core
{
namespace world
{
Node::Node(Type type, glm::vec2 pos)
: type(type)
, position(pos)
, distanceToCover{}
, occupiedBy(bl::ecs::InvalidEntity)
, targetedBy(bl::ecs::InvalidEntity) {}

float Node::getDistanceToCover(float a) const {
    a = std::fmod(a, 360.f);
    if (a < 0.f) { a += 360.f; }

    const unsigned int i = a / 30.f;
    const float weight   = std::fmod(a, 30.f) / 30.f;
    return distanceToCover[i] * (1.f - weight) + distanceToCover[(i + 1) % 12] * weight;
}

} // namespace world
} // namespace core

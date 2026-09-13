#ifndef CORE_ARENA_TRACKNODE_HPP
#define CORE_ARENA_TRACKNODE_HPP

#include <BLIB/Math/CatmullRomSegment.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
/**
 * @brief Represents a node of a train track
 *
 * @ingroup Arena
 */
struct TrackNode {
    glm::vec3 position;
    float accumulatedDistance;
    float length;
    bl::math::CatmullRomSegment<glm::vec3> spline;
};

} // namespace arena
} // namespace core

#endif

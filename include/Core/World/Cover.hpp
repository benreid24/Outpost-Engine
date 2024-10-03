#ifndef CORE_WORLD_COVER_HPP
#define CORE_WORLD_COVER_HPP

#include <glm/glm.hpp>

namespace core
{
namespace world
{
/**
 * @brief Basic rotated rectangle cover struct for prototyping
 *
 * @ingroup World
 */
struct Cover {
    glm::vec2 center;
    glm::vec2 size;
    float angle;
};

} // namespace world
} // namespace core

#endif

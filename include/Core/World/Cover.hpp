#ifndef CORE_WORLD_COVER_HPP
#define CORE_WORLD_COVER_HPP

#include <BLIB/ECS/Entity.hpp>
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
    bl::ecs::Entity entity;
    glm::vec2 center;
    glm::vec2 size;
    float angle;

    /**
     * @brief Creates an empty cover object
     */
    Cover()
    : center(0.f)
    , size(0.f)
    , angle(0.f) {}

    /**
     * @brief Creates a cover object
     *
     * @param entity The entity of the world cover
     * @param center The center of the cover in world coordinates
     * @param size The size of the cover in world coordinates
     * @param angle The rotation of the cover in degrees
     */
    Cover(bl::ecs::Entity entity, const glm::vec2& center, const glm::vec2& size, float angle)
    : entity(entity)
    , center(center)
    , size(size)
    , angle(angle) {}
};

} // namespace world
} // namespace core

#endif

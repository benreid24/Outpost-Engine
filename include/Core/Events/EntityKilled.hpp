#ifndef CORE_EVENTS_ENTITYKILLED_HPP
#define CORE_EVENTS_ENTITYKILLED_HPP

#include <BLIB/ECS/Entity.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when an entity is killed
 *
 * @ingroup Events
 */
struct EntityKilled {
    bl::ecs::Entity victim;
    bl::ecs::Entity killer;
};

} // namespace event
} // namespace core

#endif

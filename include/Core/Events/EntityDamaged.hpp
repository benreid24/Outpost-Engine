#ifndef CORE_EVENTS_ENTITYDAMAGED_HPP
#define CORE_EVENTS_ENTITYDAMAGED_HPP

#include <BLIB/ECS/Entity.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when an entity is damaged
 *
 * @ingroup Events
 */
struct EntityDamaged {
    bl::ecs::Entity victim;
    bl::ecs::Entity damager;
};

} // namespace event
} // namespace core

#endif

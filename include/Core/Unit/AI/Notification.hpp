#ifndef CORE_UNIT_AI_NOTIFICATION_HPP
#define CORE_UNIT_AI_NOTIFICATION_HPP

#include <BLIB/ECS/Entity.hpp>

namespace core
{
namespace unit
{
namespace ai
{
/**
 * @brief Special event that is passed to unit AI when events on that unit occur
 *
 * @ingroup Unit
 */
class Notification {
public:
    enum Type {
        DamageGiven,
        DamageReceived,
        KilledTarget,
    };

    Type type;
    union {
        bl::ecs::Entity damagedBy;
        bl::ecs::Entity damaged;
        bl::ecs::Entity killed;
    };
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

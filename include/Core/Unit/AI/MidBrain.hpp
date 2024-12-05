#ifndef CORE_UNIT_AI_MIDBRAIN_HPP
#define CORE_UNIT_AI_MIDBRAIN_HPP

#include <Core/Unit/AI/LowBrain.hpp>
#include <Core/World/Path.hpp>
#include <variant>

namespace core
{
namespace com
{
class Unit;
}
namespace unit
{
namespace ai
{
class MidBrain {
public:
    enum State { Idle, MovingToPosition, MovingToKill, ShootingToKill, Done = Idle, Failed };

    MidBrain(com::Unit& owner, LowBrain& lowBrain);

    /**
     * @brief Returns the current state of the brain
     */
    State getState() const { return state; }

    /**
     * @brief Performs the mid level AI update
     *
     * @param dt Time elapsed, in seconds, since the last call to update
     */
    void update(float dt);

    /**
     * @brief Notify the brain of an event affecting its unit
     *
     * @param notification The notification to process
     */
    void notify(const Notification& notification);

    void attackTarget(com::Combatant* target);

private:
    com::Unit& owner;
    LowBrain& lowBrain;
    State state;

    world::Path path;
    com::Unit* target; // TODO - target base
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

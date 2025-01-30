#ifndef CORE_UNIT_AI_MIDBRAIN_HPP
#define CORE_UNIT_AI_MIDBRAIN_HPP

#include <Core/Unit/AI/LowBrain.hpp>
#include <Core/Unit/AI/UpdateContext.hpp>
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
/**
 * @brief Mid level AI behavior for units
 *
 * @ingroup Unit
 */
class MidBrain {
public:
    /// States that the brain can be in
    enum State {
        /// The brain is waiting for a command
        Idle,

        /// The brain is path finding to a position
        MovingToPosition,

        /// The brain is repositioning to fire on a target
        MovingToKill,

        /// The brain is shooting a target
        ShootingToKill,

        /// The brain could not complete its prior command
        Failed,

        /// The brain completed its last command
        Done = Idle
    };

    /**
     * @brief Creates the mid level brain
     *
     * @param owner The unit to be controlled
     * @param lowBrain The low brain to use
     */
    MidBrain(com::Unit& owner, LowBrain& lowBrain);

    /**
     * @brief Returns the current state of the brain
     */
    State getState() const { return state; }

    /**
     * @brief Performs the mid level AI update
     *
     * @param ctx The update context
     */
    void update(const UpdateContext& ctx);

    /**
     * @brief Notify the brain of an event affecting its unit
     *
     * @param notification The notification to process
     */
    void notify(const Notification& notification);

    /**
     * @brief Cancels the current action of the brain
     */
    void makeIdle();

    /**
     * @brief Commands the brain to attack a target, possibly moving to get a better angle
     *
     * @param target The target to attack
     */
    void attackTarget(com::Combatant* target);

    /**
     * @brief Commands the brain to move to the position. Does pathfinding
     *
     * @param position The position to move to
     */
    void moveToPosition(const glm::vec2& position);

private:
    com::Unit& owner;
    LowBrain& lowBrain;
    State state;

    glm::vec2 destPos;
    world::Path path;
    bl::ecs::Entity targetEntity;
    com::Combatant* target;
    bool waitingForLowBrain;
    float waitingTime;

    void enterState(State newState);
    void processMovement(const UpdateContext& ctx);
    void processShooting(const UpdateContext& ctx);
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

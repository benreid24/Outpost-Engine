#ifndef CORE_UNIT_AI_LOWBRAIN_HPP
#define CORE_UNIT_AI_LOWBRAIN_HPP

#include <Core/Components/Combatant.hpp>
#include <Core/Unit/AI/Notification.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace com
{
class Unit;
}

namespace unit
{
/// Classes and functionality for unit and squad AI
namespace ai
{
/**
 * @brief Low level AI behavior for units
 *
 * @ingroup Unit
 */
class LowBrain {
public:
    /**
     * @brief The state of the low brain state machine
     */
    enum State {
        /// Low brain is awaiting orders from mid brain
        Idle,

        /// Low brain is moving to a position
        Moving,

        // Low brain is firing at a target
        Firing,

        // Low brain is done with prior order
        Done = Idle,

        /// Low brain could not complete command
        Failed,
    };

    /**
     * @brief Creates the low level brain
     *
     * @param unit The unit being controlled
     */
    LowBrain(com::Unit& unit);

    /**
     * @brief Returns the current state of the brain
     */
    State getState() const { return state; }

    /**
     * @brief Performs the low level AI update
     *
     * @param dt Time elapsed, in seconds, since the last call to update
     */
    void update(float dt);

    /**
     * @brief Notify the low brain of an event affecting its unit
     *
     * @param notification The notification to process
     */
    void notify(const Notification& notification);

    /**
     * @brief Commands the low level brain to move directly to the given position. Does not path
     * find
     *
     * @param position The position to move to, in world coordinates
     */
    void moveToPosition(const glm::vec2& position);

    /**
     * @brief Commands the low level brain to fire at the given target until it is dead. May go
     *        forever if the firing line becomes obstructed
     *
     * @param target The target to fire at and kill
     */
    void fireAtTarget(com::Combatant* target);

private:
    State state;
    com::Unit& owner;
    union {
        glm::vec2 targetPosition;
        struct {
            bl::ecs::Entity entity;
            com::Combatant* target;
        } killTarget;
    };

    void processMove(float dt);
    void processFire(float dt);
    void rotate(float targetAngle, float dt);
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

#ifndef CORE_UNIT_AI_TOPBRAIN_HPP
#define CORE_UNIT_AI_TOPBRAIN_HPP

#include <Core/Commands/UnitCommand.hpp>
#include <Core/Unit/AI/MidBrain.hpp>

namespace core
{
namespace unit
{
namespace ai
{
/**
 * @brief Top level AI behavior for units
 *
 * @ingroup Unit
 */
class TopBrain {
public:
    /// The state of the most recently issued command
    enum struct CommandState { Done, Executing, Failed };

    /// The state of the AI
    enum State {
        // Idle states
        Idle,
        IdlePursuing,
        IdleRetreating,

        // Moving states
        Moving,
        // TODO - need breakdown?

        // Attacking states
        Attacking,
        // TODO - need breakdown?

        // Suppressing states
        Suppressing,
        SuppressPursuing,
        SuppressRetreating,
    };

    /**
     * @brief Creates the top level brain
     *
     * @param owner The unit being controlled
     * @param midBrain The mid level brain of the unit
     */
    TopBrain(com::Unit& owner, MidBrain& midBrain);

    /**
     * @brief Returns the current state of the brain
     */
    State getState() const { return state; }

    /**
     * @brief Returns the execution state of the most recent command
     */
    CommandState getCommandState() const { return commandState; }

    /**
     * @brief Performs the top level AI update
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
     * @brief Issues a command to the brain
     *
     * @param cmd The command to execute
     */
    void issueCommand(const cmd::UnitCommand& cmd);

    /**
     * @brief Cancels the current action of the brain
     *
     * @param commandState The new command state to enter
     */
    void makeIdle(CommandState commandState = CommandState::Done);

private:
    com::Unit& owner;
    MidBrain& midBrain;
    CommandState commandState;
    State state;
    cmd::AggroLevel stance;

    void checkMidBrainResult();
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

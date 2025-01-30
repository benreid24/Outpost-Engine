#ifndef CORE_COMMANDS_COMMAND_HPP
#define CORE_COMMANDS_COMMAND_HPP

#include <Core/Commands/AggroLevel.hpp>

/**
 * @addtogroup Commands
 * @ingroup Core
 * @brief Functionality, storage, and events for commands for units and squads
 */

namespace core
{
/// Functionality, storage, and events for commands for units and squads
namespace cmd
{
template<typename T>
class ExecutorHandle;
template<typename T>
class ExternalHandle;
class CommandStore;

/**
 * @brief Base class for commands that can be issued to units and squads
 *
 * @ingroup Commands
 */
class Command {
public:
    /**
     * @brief The execution status of a command
     */
    enum Status { Invalid, Queued, Current, Complete, Failed, Canceled };

    /**
     * @brief Returns the status of this command
     */
    Status getStatus() const { return status; }

    /**
     * @brief Returns how aggressively the command should be executed
     */
    AggroLevel getAggroLevel() const { return aggroLevel; }

    /**
     * @brief Returns whether this command is terminated or not
     */
    bool isTerminal() const { return isTerminal(status); }

    /**
     * @brief Tests whether the given command status is terminal or not
     *
     * @param status The status to check
     * @return Whether the command has completed running or not
     */
    static bool isTerminal(Status status) { return status >= Complete; }

protected:
    /**
     * @brief Creates the command in the Invalid state
     */
    Command(AggroLevel aggroLevel)
    : status(Invalid)
    , aggroLevel(aggroLevel) {}

private:
    Status status;
    AggroLevel aggroLevel;

    template<typename T>
    friend class ExecutorHandle;
    template<typename T>
    friend class ExternalHandle;
    friend class CommandStore;
};

} // namespace cmd
} // namespace core

#endif

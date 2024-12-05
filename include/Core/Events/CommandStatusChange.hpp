#ifndef CORE_EVENTS_COMMANDSTATUSCHANGE_HPP
#define CORE_EVENTS_COMMANDSTATUSCHANGE_HPP

/**
 * @addtogroup Events
 * @ingroup Core
 * @brief Collection of events that are fired on the engine event bus
 */

#include <Core/Commands/Command.hpp>

namespace core
{
/// Namespace containing event bus structs
namespace event
{
/**
 * @brief Event that is fired when a command changes state. Not all states result in events being
 *        fired. Subscribable states are Queued, Current, Complete, Failed, Canceled
 *
 * @tparam T The command type to get events for
 * @tparam Status The status to get events for
 * @ingroup Commands
 * @ingroup Events
 */
template<typename T, cmd::Command::Status Status>
struct CommandStatusChange {
    static_assert(Status >= cmd::Command::Queued,
                  "Status must be one of Queued, Current, Complete, Failed, Canceled");

    const T& command;
};

} // namespace event
} // namespace core

#endif

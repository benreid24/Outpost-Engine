#ifndef CORE_COMMANDS_EVENTS_HPP
#define CORE_COMMANDS_EVENTS_HPP

#include <Core/Commands/Command.hpp>

namespace core
{
namespace cmd
{
/**
 * @brief Event that is fired when a command changes state. Not all states result in events being
 *        fired. Subscribable states are Queued, Current, Complete, Failed, Canceled
 *
 * @tparam T The command type to get events for
 * @tparam Status The status to get events for
 * @ingroup Commands
 */
template<typename T, Command::Status Status>
struct CommandStatusChange {
    static_assert(Status >= Command::Queued,
                  "Status must be one of Queued, Current, Complete, Failed, Canceled");

    const T& command;
};

} // namespace cmd
} // namespace core

#endif

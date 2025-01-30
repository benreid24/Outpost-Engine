#ifndef CORE_COMMANDS_ADDMODE_HPP
#define CORE_COMMANDS_ADDMODE_HPP

namespace core
{
namespace cmd
{
/**
 * @brief Ways that commands can be added to queues
 *
 * @ingroup Commands
 */
enum struct AddMode {
    /// The command is added to the end of the queue
    QueueEnd,

    /// The command is added to the front of the queue
    QueueStart,

    /// The command replaces the current command (moving it to front of queue)
    Immediate,

    /// The command replaces the current command and the queue is emptied
    ReplaceAll
};

} // namespace cmd
} // namespace core

#endif

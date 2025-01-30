#ifndef CORE_COMMANDS_AGGROLEVEL_HPP
#define CORE_COMMANDS_AGGROLEVEL_HPP

namespace core
{
namespace cmd
{
/**
 * @brief The aggressiveness to perform commands with
 *
 * @ingroup Commands
 */
enum struct AggroLevel {
    /// Unit will go out of its way to attack while performing command
    Aggressive,

    /// Unit will perform command, only attacking defensively
    Neutral,

    /// Unit will perform command but retreat if attacked
    Coward
};

} // namespace cmd
} // namespace core

#endif

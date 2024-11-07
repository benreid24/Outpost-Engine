#ifndef CORE_UNIT_COMMANDCONTEXT_HPP
#define CORE_UNIT_COMMANDCONTEXT_HPP

#include <Core/Unit/Command.hpp>
#include <variant>
#include <vector>

namespace core
{
namespace unit
{
/**
 * @brief Execution level for units executing commands
 *
 * @ingroup Unit
 */
class CommandContext {
public:
    /**
     * @brief Data for executing movement based commands
     */
    struct PathContext {
        std::vector<world::Node*> path;
        std::size_t currentNode;
        float targetAngle;
    };

    /**
     * @brief Data for executing combat based commands
     */
    struct ShootContext {};

    /**
     * @brief Initializes the context for the given command
     *
     * @param cmd The command to init data for
     */
    void init(const Command& cmd);

    /**
     * @brief Clears context data and releases memory
     */
    void clear();

    /**
     * @brief Returns the path context data
     */
    PathContext& getPathContext();

    /**
     * @brief Returns the shooting context data
     */
    ShootContext& getShootContext();

private:
    std::variant<std::monostate, PathContext, ShootContext> data;
};

} // namespace unit
} // namespace core

#endif

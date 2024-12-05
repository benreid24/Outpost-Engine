#ifndef CORE_COMMANDS_SQUADCOMMAND_HPP
#define CORE_COMMANDS_SQUADCOMMAND_HPP

#include <Core/Commands/Command.hpp>
#include <Core/Commands/ExternalHandle.hpp>
#include <Core/Components/Target.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace cmd
{
/**
 * @brief Command that can be issued to squads
 *
 * @ingroup Commands
 */
class SquadCommand : public Command {
public:
    enum struct Type { Idle, Move, Suppress, AttackArea, AttackUnit, Invalid };

    /**
     * @brief Creates a squad command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressively to perform the command
     */
    SquadCommand(Type type, AggroLevel aggroLevel)
    : Command(aggroLevel)
    , type(type) {}

    /**
     * @brief Creates a squad command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressively to perform the command
     * @param position The target position of the command
     */
    SquadCommand(Type type, AggroLevel aggroLevel, const glm::vec2& position)
    : Command(aggroLevel)
    , type(type)
    , position(position) {}

    /**
     * @brief Creates a squad command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressively to perform the command
     * @param target The target of the command
     */
    SquadCommand(Type type, AggroLevel aggroLevel, com::Target* target)
    : Command(aggroLevel)
    , type(type)
    , target(target) {}

    /**
     * @brief Returns the type of this command
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the target position of the command
     */
    const glm::vec2& getTargetPosition() { return position; }

private:
    Type type;
    union {
        glm::vec2 position;
        com::Target* target;
    };
};

/**
 * @brief Handle to a squad command
 *
 * @ingroup Commands
 */
using SquadCommandHandle = ExternalHandle<SquadCommand>;

} // namespace cmd
} // namespace core

#endif

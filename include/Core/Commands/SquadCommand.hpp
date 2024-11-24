#ifndef CORE_COMMANDS_SQUADCOMMAND_HPP
#define CORE_COMMANDS_SQUADCOMMAND_HPP

#include <Core/Commands/Command.hpp>
#include <Core/Commands/ExternalHandle.hpp>
#include <Core/Components/Unit.hpp>
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
    enum struct Type {
        MoveToPosition,
        AttackArea,
        AttackUnit,
        // TODO - other commands
        Invalid
    };

    /**
     * @brief Creates a squad command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param position The target position of the command
     */
    SquadCommand(Type type, const glm::vec2& position)
    : type(type)
    , position(position) {}

    /**
     * @brief Creates a squad command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param unit The target unit of the command
     */
    SquadCommand(Type type, com::Unit* unit)
    : type(type)
    , unit(unit) {}

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
        com::Unit* unit;
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

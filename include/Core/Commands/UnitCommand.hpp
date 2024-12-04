#ifndef CORE_COMMANDS_UNITCOMMAND_HPP
#define CORE_COMMANDS_UNITCOMMAND_HPP

#include <Core/Commands/Command.hpp>
#include <Core/Commands/ExternalHandle.hpp>
#include <Core/World/Node.hpp>

namespace core
{
namespace com
{
class Unit;
}

namespace cmd
{
/**
 * @brief Commands that can be issued to units
 *
 * @ingroup Unit
 * @ingroup Commands
 */
class UnitCommand : public Command {
public:
    /**
     * @brief The actual type of the command
     */
    enum struct Type { Idle, Move, Attack, Suppress, Invalid };

    /**
     * @brief Creates a unit command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressive to be when doing the command
     */
    UnitCommand(Type type, AggroLevel aggroLevel)
    : Command(aggroLevel)
    , type(type) {}

    /**
     * @brief Creates a unit command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressive to be when doing the command
     * @param position The position of the command
     */
    UnitCommand(Type type, AggroLevel aggroLevel, const glm::vec2& position)
    : Command(aggroLevel)
    , type(type) {
        targetPosition = position;
    }

    /**
     * @brief Creates a unit command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param aggroLevel How aggressive to be when doing the command
     * @param unit The unit of the command
     */
    UnitCommand(Type type, AggroLevel aggroLevel, com::Unit* unit)
    : Command(aggroLevel)
    , type(type) {
        targetUnit = unit;
    }

    /**
     * @brief Returns the type of this command
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the target position of this command
     */
    const glm::vec2& getTargetPosition() const { return targetPosition; }

    /**
     * @brief Returns the target unit of this command
     */
    com::Unit* getTargetUnit() const { return targetUnit; }

private:
    Type type;

    union {
        glm::vec2 targetPosition;
        com::Unit* targetUnit;
    };
};

/**
 * @brief Handle for unit commands
 *
 * @ingroup Commands
 */
using UnitCommandHandle = ExternalHandle<UnitCommand>;

} // namespace cmd
} // namespace core

#endif

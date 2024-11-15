#ifndef CORE_COMMANDS_UNITCOMMAND_HPP
#define CORE_COMMANDS_UNITCOMMAND_HPP

#include <Core/Commands/Command.hpp>
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
     * @brief The concurrency type of command. Commands of different types may be executed in
     *        parallel by a single unit
     */
    struct ConcurrencyType {
        enum Slot { Movement = 0, Shooting = 1, COUNT };
    };

    /**
     * @brief The actual type of the command
     */
    enum struct Type {
        MoveToPosition,
        KillUnit,
        // TODO - other commands
        Invalid
    };

    /**
     * @brief Creates a unit command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param concurrencyType The concurrency type of command
     * @param position The position of the command
     * @param node The world node of the command
     */
    UnitCommand(Type type, ConcurrencyType::Slot concurrencyType, const glm::vec2& position,
                const world::Node* node)
    : concurrencyType(concurrencyType)
    , type(type) {
        targetPosition.position = position;
        targetPosition.node     = node;
    }

    /**
     * @brief Creates a unit command. Use the factory methods on CommandStore
     *
     * @param type The type of command
     * @param concurrencyType The concurrency type of command
     * @param unit The unit of the command
     */
    UnitCommand(Type type, ConcurrencyType::Slot concurrencyType, com::Unit* unit)
    : type(type)
    , concurrencyType(concurrencyType) {
        targetUnit = unit;
    }

    /**
     * @brief Returns the type of this command
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the concurrency type for this command
     */
    ConcurrencyType::Slot getConcurrencyType() const { return concurrencyType; }

    /**
     * @brief Returns the target world node of this command
     */
    const world::Node* getTargetNode() const { return targetPosition.node; }

    /**
     * @brief Returns the target position of this command
     */
    const glm::vec2& getTargetPosition() const { return targetPosition.position; }

    /**
     * @brief Returns the target unit of this command
     */
    com::Unit* getTargetUnit() const { return targetUnit; }

private:
    ConcurrencyType::Slot concurrencyType;
    Type type;

    union {
        struct {
            glm::vec2 position;
            const world::Node* node;

        } targetPosition;

        com::Unit* targetUnit;
    };
};

} // namespace cmd
} // namespace core

#endif

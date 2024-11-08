#ifndef CORE_UNIT_COMMAND_HPP
#define CORE_UNIT_COMMAND_HPP

#include <Core/World/Node.hpp>

namespace core
{
namespace com
{
class Unit;
}

namespace unit
{
/**
 * @brief Commands that can be issued to units
 *
 * @ingroup Unit
 */
class Command {
public:
    /**
     * @brief The execution status of a unit command
     */
    enum Status { Queued, Current, Complete, Failed };

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
        MoveToNode,
        KillUnit,
        // TODO - other commands
        Invalid
    };

    /**
     * @brief Creates an invalid, empty command
     */
    Command();

    /**
     * @brief Creates a command to move to a node
     *
     * @param target The world node to move to
     * @return The command to move to the node
     */
    static Command makeMoveToNodeCommand(const world::Node* target);

    /**
     * @brief Creates a command to kill a unit
     *
     * @param entity The ECS id of the unit to kill
     * @param unit The unit component of the unit to kill
     * @return The command to kill the unit
     */
    static Command makeKillUnitCommand(bl::ecs::Entity entity, com::Unit* unit);

    /**
     * @brief Returns the concurrency type for this command
     */
    ConcurrencyType::Slot getConcurrencyType() const { return concurrencyType; }

    /**
     * @brief Returns the type of this command
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the status of this command
     */
    Status getStatus() const { return status; }

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

    /**
     * @brief Returns the target world node of this command
     */
    const world::Node* getTargetNode() const { return targetNode; }

    /**
     * @brief Returns the target unit entity id of this command
     */
    bl::ecs::Entity getTargetUnitEntity() const { return targetUnit.entity; }

    /**
     * @brief Returns the target unit of this command
     */
    com::Unit* getTargetUnit() const { return targetUnit.unit; }

    /**
     * @brief Sets the status to Current if Queued
     */
    void markInProgress();

    /**
     * @brief Sets the status to Complete if Current
     */
    void markComplete();

    /**
     * @brief Sets the status to Failed if Current
     */
    void markFailed();

    /**
     * @brief Copies the command
     *
     * @param copy The command to copy
     */
    Command(const Command& copy);

    /**
     * @brief Copies the command
     *
     * @param copy The command to copy
     * @return A reference to this command
     */
    Command& operator=(const Command& copy);

private:
    Status status;
    ConcurrencyType::Slot concurrencyType;
    Type type;

    union {
        const world::Node* targetNode;
        struct {
            bl::ecs::Entity entity;
            com::Unit* unit;
        } targetUnit;
    };
};

} // namespace unit
} // namespace core

#endif

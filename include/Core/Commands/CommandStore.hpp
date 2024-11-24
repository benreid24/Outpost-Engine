#ifndef CORE_COMMANDS_COMMANDSTORE_HPP
#define CORE_COMMANDS_COMMANDSTORE_HPP

#include <Core/Commands/ExternalHandle.hpp>
#include <Core/Commands/SingleStore.hpp>
#include <Core/Commands/SquadCommand.hpp>
#include <Core/Commands/UnitCommand.hpp>

namespace core
{
namespace cmd
{
/**
 * @brief Top level command store class. Provides persistent storage for commands so that observers,
 *        UI, AI, etc, can maintain handles after executors have completed
 *
 * @ingroup Commands
 */
class CommandStore {
public:
    /**
     * @brief Creates the command store
     */
    CommandStore();

    /**
     * @brief Makes a unit move command
     *
     * @param position The position to move to
     * @param node The world node to move to
     * @return A handle to the new move command
     */
    UnitCommandHandle unitMakeMove(const glm::vec2& position, const world::Node* node);

    /**
     * @brief Makes a unit attack command
     *
     * @param target The unit to attack
     * @return A handle to the new attack command
     */
    UnitCommandHandle unitMakeAttack(com::Unit* target);

    /**
     * @brief Makes a squad move command
     *
     * @param position The position to move to
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeMove(const glm::vec2& position);

    /**
     * @brief Makes a squad attack command
     *
     * @param position The position to suppress
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeAttack(const glm::vec2& position);

    /**
     * @brief Makes a squad attack command
     *
     * @param unit The unit to attack
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeAttack(com::Unit* unit);

private:
    SingleStore<UnitCommand> unitCommands;
    SingleStore<SquadCommand> squadCommands;
};

} // namespace cmd
} // namespace core

#endif

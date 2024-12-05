#ifndef CORE_COMMANDS_COMMANDSTORE_HPP
#define CORE_COMMANDS_COMMANDSTORE_HPP

#include <Core/Commands/ExternalHandle.hpp>
#include <Core/Commands/SingleStore.hpp>
#include <Core/Commands/SquadCommand.hpp>
#include <Core/Commands/UnitCommand.hpp>
#include <Core/Components/Target.hpp>

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
     * @param aggroLevel How aggressively to move
     * @return A handle to the new move command
     */
    UnitCommandHandle unitMakeMove(const glm::vec2& position, AggroLevel aggroLevel);

    /**
     * @brief Makes a unit attack command
     *
     * @param target The target to attack
     * @return A handle to the new attack command
     */
    UnitCommandHandle unitMakeAttack(com::Target* target, AggroLevel aggroLevel);

    /**
     * @brief Makes a unit idle command
     *
     * @param aggroLevel How aggressively to idle
     * @param aggroLevel How aggressively to attack
     * @return A handle to the new idle command
     */
    UnitCommandHandle unitMakeIdle(AggroLevel aggroLevel);

    /**
     * @brief Makes a new unit suppress command
     *
     * @param position The position to suppress
     * @param aggroLevel How aggressively to suppress
     * @return A handle to the new unit suppress command
     */
    UnitCommandHandle unitMakeSuppress(const glm::vec2& position, AggroLevel aggroLevel);

    /**
     * @brief Makes a squad idle command
     *
     * @param aggroLevel How aggressively to idle
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeIdle(AggroLevel aggroLevel);

    /**
     * @brief Makes a squad move command
     *
     * @param position The position to move to
     * @param aggroLevel How aggressively to move
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeMove(const glm::vec2& position, AggroLevel aggroLevel);

    /**
     * @brief Makes a squad attack command
     *
     * @param position The position to attack
     * @param aggroLevel How aggressively to attack
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeAttack(const glm::vec2& position, AggroLevel aggroLevel);

    /**
     * @brief Makes a squad suppress command
     *
     * @param position The position to suppress
     * @param aggroLevel How aggressively to suppress
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeSuppress(const glm::vec2& position, AggroLevel aggroLevel);

    /**
     * @brief Makes a squad attack command
     *
     * @param target The target to attack
     * @param aggroLevel How aggressively to attack
     * @return A handle to the new command
     */
    SquadCommandHandle squadMakeAttack(com::Target* target, AggroLevel aggroLevel);

private:
    SingleStore<UnitCommand> unitCommands;
    SingleStore<SquadCommand> squadCommands;
};

} // namespace cmd
} // namespace core

#endif

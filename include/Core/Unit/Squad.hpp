#ifndef CORE_UNIT_SQUAD_HPP
#define CORE_UNIT_SQUAD_HPP

#include <BLIB/Containers/StaticRingBuffer.hpp>
#include <Core/Commands/Executor.hpp>
#include <Core/Commands/SquadCommand.hpp>
#include <Core/Components/Unit.hpp>
#include <vector>

namespace core
{
namespace unit
{
/**
 * @brief A collection of units that can be issued higher level commands
 *
 * @ingroup Units
 */
class Squad : public cmd::Executor<cmd::SquadCommand> {
public:
    static constexpr std::size_t CommandQueueSize = 4;

    /**
     * @brief Creates a new empty squad
     *
     * @param faction The faction of the squad
     */
    Squad(fcn::FactionId faction);

    /**
     * @brief Returns the faction that this squad belongs to
     */
    fcn::FactionId getFaction() const { return faction; }

    /**
     * @brief Adds a unit to this squad
     *
     * @param unit The unit to add
     * @return Whether the unit is now in the squad
     */
    bool addUnit(com::Unit* unit);

    /**
     * @brief Removes the unit from the squad
     *
     * @param unit The unit to remove
     */
    void removeUnit(com::Unit* unit);

    /**
     * @brief Removes all units from the squad
     */
    void clearUnits();

    /**
     * @brief Queues a command to the squad
     *
     * @param command The command to execute
     * @return Whether the command was able to be queued
     */
    bool queueCommand(const cmd::SquadCommandHandle& command);

private:
    using CmdHandle = cmd::ExecutorHandle<cmd::SquadCommand>;

    fcn::FactionId faction;
    std::vector<com::Unit*> units;
    CmdHandle currentCommand;
    bl::ctr::StaticRingBuffer<CmdHandle, CommandQueueSize> queuedCommands;
};

} // namespace unit
} // namespace core

#endif

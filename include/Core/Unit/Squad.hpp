#ifndef CORE_UNIT_SQUAD_HPP
#define CORE_UNIT_SQUAD_HPP

#include <BLIB/Containers/StaticRingBuffer.hpp>
#include <Core/Commands/Queue.hpp>
#include <Core/Commands/SquadCommand.hpp>
#include <Core/Components/Unit.hpp>
#include <Core/Components/UnitAI.hpp>
#include <vector>

namespace core
{
namespace unit
{
class SquadManager;

/**
 * @brief A collection of units that can be issued higher level commands
 *
 * @ingroup Units
 */
class Squad {
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
     * @param addMode How to add the command to the queue
     */
    void queueCommand(const cmd::SquadCommandHandle& command,
                      cmd::AddMode addMode = cmd::AddMode::QueueEnd);

private:
    struct SquadUnit {
        com::Unit* unit;
        com::UnitAI* ai;
    };

    fcn::FactionId faction;
    std::vector<SquadUnit> units;
    cmd::Queue<cmd::SquadCommand, CommandQueueSize> commandQueue;
    std::vector<cmd::UnitCommandHandle> unitCommands;

    friend class SquadManager;
};

} // namespace unit
} // namespace core

#endif

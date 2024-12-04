#ifndef CORE_COMPONENTS_UNIT_HPP
#define CORE_COMPONENTS_UNIT_HPP

#include <BLIB/Components/Physics2D.hpp>
#include <Core/Commands/Queue.hpp>
#include <Core/Commands/UnitCommand.hpp>
#include <Core/Factions/FactionId.hpp>
#include <Core/Unit/Capabilities.hpp>
#include <Core/Unit/CommandContext.hpp>

/**
 * @addtogroup Unit
 * @ingroup Core
 * @brief Classes and behaviors for in-game units
 */

namespace core
{
namespace sys
{
class Unit;
}
namespace com
{
/**
 * @brief Component and data storage for in game units
 *
 * @ingroup Components
 * @ingroup Unit
 */
class Unit {
public:
    static constexpr std::size_t CommandQueueSize = 4;

    /**
     * @brief Creates a unit that cannot move or shoot
     *
     * @param faction The id of the faction that this unit belongs to
     * @param physics The physics component of the unit
     */
    Unit(fcn::FactionId faction, bl::com::Physics2D& physics);

    /**
     * @brief Returns the entity id of this unit
     */
    bl::ecs::Entity getId() const { return physics.getOwner(); }

    /**
     * @brief Returns the id of the faction the unit belongs to
     */
    fcn::FactionId getFaction() const { return faction; }

    /**
     * @brief Returns the units capabilities
     */
    unit::Capabilities& capabilities() { return abilities; }

    /**
     * @brief Queues a command to be carried out by the unit
     *
     * @param command The command to perform
     * @param addMode How to add the command to the queue
     */
    void queueCommand(const cmd::UnitCommandHandle& command,
                      cmd::AddMode addMode = cmd::AddMode::QueueEnd);

private:
    // unit data and components
    fcn::FactionId faction;
    bl::com::Physics2D& physics;
    unit::Capabilities abilities;

    // unit AI & command state
    cmd::Queue<cmd::UnitCommand, CommandQueueSize> commandQueue;
    // TODO - AI state

    friend class sys::Unit;
};

} // namespace com
} // namespace core

#endif

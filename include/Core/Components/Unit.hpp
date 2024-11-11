#ifndef CORE_COMPONENTS_UNIT_HPP
#define CORE_COMPONENTS_UNIT_HPP

#include <BLIB/Containers/StaticRingBuffer.hpp>
#include <Core/Unit/Command.hpp>
#include <Core/Unit/CommandContext.hpp>
#include <Core/Unit/Moveable.hpp>
#include <Core/Unit/Shooter.hpp>
#include <Core/World/Node.hpp>
#include <optional>

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
class Unit {
public:
    static constexpr std::size_t CommandQueueSize = 4;

    /**
     * @brief Creates a unit that cannot move or shoot
     *
     * @param physics The physics component of the unit
     */
    Unit(bl::com::Physics2D& physics);

    /**
     * @brief Returns the entity id of this unit
     */
    bl::ecs::Entity getId() const { return physics.getOwner(); }

    /**
     * @brief Allows the unit to move with the following parameters
     *
     * @param acceleration The rate of acceleration for movement, in world space units
     * @param maxSpeed The maximum movement speed to enforce, in world space units
     * @param rotateRate The rotation speed in degrees per second
     * @param directionAdjustSpeed How quickly the entity velocity changes direction. In range [0,1]
     * @param damping The damping factor to use to stop the entity when not moving
     */
    void makeMoveable(float acceleration, float maxSpeed, float rotateRate,
                      float directionAdjustSpeed, float damping = 10.f);

    /**
     * @brief Allows the unit to shoot with the following parameters
     *
     * @param fireRate The fire rate in bullets per second
     * @param damage The damage applied by each bullet
     * @param bulletOffset The distance from the shooter to spawn bullets
     */
    void makeShooter(float fireRate, float damage, float bulletOffset);

    /**
     * @brief Returns whether this unit can move or not
     */
    bool canMove() const { return mover.has_value(); }

    /**
     * @brief Returns the movement component for this unit. Only call if this unit can move
     */
    unit::Moveable& getMover() { return mover.value(); }

    /**
     * @brief Returns whether this unit can fire or not
     */
    bool canShoot() const { return shooter.has_value(); }

    /**
     * @brief Returns the shooter component for this unit. Only call if this unit can fire
     */
    unit::Shooter& getShooter() { return shooter.value(); }

    /**
     * @brief Queues a command to be carried out by the unit
     *
     * @param command The command to perform
     * @return Whether or not the command was able to be queued
     */
    bool queueCommand(const unit::Command& command);

private:
    // unit data and components
    bl::com::Physics2D& physics;
    std::optional<unit::Moveable> mover;
    std::optional<unit::Shooter> shooter;

    // unit AI & command state
    unit::Command activeCommands[unit::Command::ConcurrencyType::COUNT];
    unit::CommandContext commandStates[unit::Command::ConcurrencyType::COUNT];
    bl::ctr::StaticRingBuffer<unit::Command, CommandQueueSize> queuedCommands;

    friend class sys::Unit;
};

} // namespace com
} // namespace core

#endif

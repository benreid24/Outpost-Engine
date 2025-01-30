#ifndef CORE_UNIT_SHOOTER_HPP
#define CORE_UNIT_SHOOTER_HPP

#include <Core/Properties.hpp>

namespace core
{
namespace sys
{
class Unit;
}

/// Collection of functionality and behaviors for units
namespace unit
{
namespace able
{
/**
 * @brief Component that allows a unit to fire bullets
 *
 * @ingroup Unit
 */
class Shoot {
public:
    /**
     * @brief Creates the shooter component with default values
     */
    Shoot()
    : fireDelay(1.f / Properties.UnitFireRateDefault.get())
    , damage(Properties.UnitBulletDamageDefault.get())
    , bulletOffset(Properties.UnitBulletOffsetDefault.get())
    , timeSinceLastFire(fireDelay)
    , isFiring(false) {}

    /// Time in seconds between bullets firing
    float fireDelay;

    /// How much damage a bullet does
    float damage;

    /// Distance in world units to offset from shooter when spawning bullets
    float bulletOffset;

    /**
     * @brief Attempts to fire the weapon
     */
    void fire() { isFiring = true; }

private:
    float timeSinceLastFire;
    bool isFiring;

    friend class sys::Unit;
};

} // namespace able
} // namespace unit
} // namespace core

#endif

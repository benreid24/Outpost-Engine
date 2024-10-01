#ifndef CORE_COMPONENTS_SHOOTER_HPP
#define CORE_COMPONENTS_SHOOTER_HPP

namespace core
{
namespace sys
{
class EntityActions;
}

namespace com
{
/**
 * @brief Component that allows an entity to fire bullets
 *
 * @ingroup Components
 */
class Shooter {
public:
    /**
     * @brief Creates the shooter component
     *
     * @param fireRate The fire rate in bullets per second
     * @param damage The damage applied by each bullet
     * @param bulletOffset The distance from the shooter to spawn bullets
     */
    Shooter(float fireRate, float damage, float bulletOffset)
    : fireDelay(1.f / fireRate)
    , damage(damage)
    , bulletOffset(bulletOffset)
    , timeSinceLastFire(1.f / fireRate)
    , isFiring(false) {}

    /**
     * @brief Returns the fire rate in bullets per second
     */
    float getFireRate() const { return 1.f / fireDelay; }

    /**
     * @brief Returns the delay between bullets in seconds
     */
    float getFireDelay() const { return fireDelay; }

    /**
     * @brief Returns the damage of bullets
     */
    float getDamage() const { return damage; }

    /**
     * @brief Returns the offset to spawn bullets from the shooter
     */
    float getBulletOffset() const { return bulletOffset; }

    /**
     * @brief Attempts to fire the weapon
     */
    void fire() { isFiring = true; }

private:
    float fireDelay;
    float damage;
    float bulletOffset;
    float timeSinceLastFire;
    bool isFiring;

    friend class sys::EntityActions;
};

} // namespace com
} // namespace core

#endif

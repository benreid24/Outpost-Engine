#ifndef CORE_COMPONENTS_DAMAGER_HPP
#define CORE_COMPONENTS_DAMAGER_HPP

namespace core
{
namespace com
{
/**
 * @brief Component that makes an entity cause damage on collision
 *
 * @ingroup Components
 */
struct Damager {
    float damage;

    /**
     * @brief Creates the damager component with 0 damage
     */
    Damager()
    : damage(0.f) {}

    /**
     * @brief Creates the damager component with the given damage
     *
     * @param damage The damage to apply on collision
     */
    Damager(float damage)
    : damage(damage) {}
};

} // namespace com
} // namespace core

#endif

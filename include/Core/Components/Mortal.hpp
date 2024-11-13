#ifndef CORE_COMPONENTS_MORTAL_HPP
#define CORE_COMPONENTS_MORTAL_HPP

/**
 * @addtogroup Components
 * @ingroup Core
 * @brief Collection of core game components
 */

namespace core
{
/// Collection of core game components
namespace com
{
/**
 * @brief Component that makes an entity susceptible to damage and dying
 *
 * @ingroup Components
 */
struct Mortal {
    float health;
    float deathTime;

    Mortal(float health, float deathTime = -1.f)
    : health(health)
    , deathTime(deathTime) {}
};

} // namespace com
} // namespace core

#endif

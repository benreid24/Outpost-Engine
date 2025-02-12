#ifndef CORE_COMPONENTS_COMBATANT_HPP
#define CORE_COMPONENTS_COMBATANT_HPP

#include <BLIB/Components/Physics2D.hpp>
#include <Core/Factions/FactionId.hpp>

namespace core
{
namespace com
{
/**
 * @brief Component that makes an entity target-able and kill-able
 *
 * @ingroup Components
 */
class Combatant {
public:
    /**
     * @brief Creates the target component
     *
     * @param faction The faction that the combatant belongs to
     * @param physics The physics component of the target
     * @param health The health of the target
     * @param deathTime How long the body should stay after death
     */
    Combatant(fcn::FactionId faction, bl::com::Physics2D& physics, float health,
              float deathTime = 0.f)
    : faction(faction)
    , physics(physics)
    , hp(health)
    , deathTime(deathTime) {}

    /**
     * @brief Returns the entity id of this unit
     */
    bl::ecs::Entity getId() const { return physics.getOwner(); }

    /**
     * @brief Returns the faction this combatant belongs to
     */
    fcn::FactionId getFaction() const { return faction; }

    /**
     * @brief Returns the position of the target in world coordinates
     */
    glm::vec2 getPosition() const { return physics.getTransform().getGlobalPosition(); }

    /**
     * @brief Returns the rotation of the target in degrees
     */
    float getRotation() const { return physics.getTransform().getRotation(); }

    /**
     * @brief Returns the current amount of health of the target
     */
    float getHealth() const { return hp; }

    /**
     * @brief Returns whether the target is still alive
     */
    bool isAlive() const { return hp > 0.f; }

    /**
     * @brief Returns the time left before the body disappears
     */
    float getDeathTime() const { return deathTime; }

    /**
     * @brief Applies damage to the target
     *
     * @param damage The damage to apply
     * @return True if the target died, false if still alive
     */
    bool applyDamage(float damage) {
        if (hp > damage) {
            hp -= damage;
            return false;
        }
        hp = 0.f;
        return true;
    }

private:
    fcn::FactionId faction;
    bl::com::Physics2D& physics;
    float hp;
    float deathTime;
};

} // namespace com
} // namespace core

#endif

#ifndef CORE_SYSTEMS_DAMAGE_HPP
#define CORE_SYSTEMS_DAMAGE_HPP

#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Systems/Physics2D.hpp>
#include <Core/Components/Damager.hpp>
#include <Core/Components/Mortal.hpp>

namespace core
{
namespace sys
{
/**
 * @brief System that applies damage to entities from collisions
 *
 * @ingroup Systems
 */
class Damage : public bl::event::Listener<bl::sys::Physics2D::EntityCollisionBeginEvent> {
public:
    /**
     * @brief Creates the system
     */
    Damage();

    /**
     * @brief Destroys the system
     */
    virtual ~Damage() = default;

    /**
     * @brief Initializes the system
     *
     * @param engine The game engine instance
     */
    void init(bl::engine::Engine& engine);

    /**
     * @brief Makes the given entity mortal
     *
     * @param entity The entity to make mortal
     * @param health The amount of health to give it
     * @param deathTime How long, in seconds, for the body to stay before being removed
     */
    void makeMortal(bl::ecs::Entity entity, float health, float deathTime = -1.f);

    /**
     * @brief Makes the given entity a giver of damage
     *
     * @param entity The entity to make a damager
     * @param damage The amount of damage for the entity to apply
     */
    void makeDamager(bl::ecs::Entity entity, float damage);

private:
    bl::engine::Engine* engine;

    using Transaction = bl::ecs::Transaction<
        bl::ecs::tx::EntityWrite, bl::ecs::tx::ComponentRead<>,
        bl::ecs::tx::ComponentWrite<com::Damager, com::Mortal, bl::com::MarkedForDeath>>;

    void applyDamage(bl::ecs::Entity mortalEntity, com::Mortal& victim,
                     bl::ecs::Entity damagerEntity, com::Damager& damager, Transaction& tx);
    virtual void observe(const bl::sys::Physics2D::EntityCollisionBeginEvent& event) override;
};

} // namespace sys
} // namespace core

#endif

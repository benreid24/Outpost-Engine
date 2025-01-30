#include <Core/Systems/Damage.hpp>

#include <Core/Events/EntityDamaged.hpp>
#include <Core/Events/EntityKilled.hpp>

namespace core
{
namespace sys
{
Damage::Damage()
: engine(nullptr) {}

void Damage::init(bl::engine::Engine& e) { engine = &e; }

void Damage::makeMortal(bl::ecs::Entity entity, fcn::FactionId faction, bl::com::Physics2D& physics,
                        float health, float deathTime) {
    engine->ecs().emplaceComponent<com::Combatant>(entity, faction, physics, health, deathTime);
}

void Damage::makeDamager(bl::ecs::Entity entity, float damage) {
    engine->ecs().emplaceComponent<com::Damager>(entity, damage);
}

void Damage::observe(const bl::sys::Physics2D::EntityCollisionBeginEvent& collision) {
    using namespace bl::ecs;
    Transaction tx(engine->ecs());

    auto setA = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Combatant>>(
        collision.entityA, tx);
    auto setB = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Combatant>>(
        collision.entityB, tx);
    tx.unlock();

    if (setB.get<com::Damager>()) {
        if (setA.get<com::Combatant>()) {
            applyDamage(collision.entityA,
                        *setA.get<com::Combatant>(),
                        collision.entityB,
                        *setB.get<com::Damager>(),
                        tx);
        }
        engine->ecs().destroyEntity(setB.entity(), tx);
    }
    if (setA.get<com::Damager>()) {
        if (setB.get<com::Combatant>()) {
            applyDamage(collision.entityB,
                        *setB.get<com::Combatant>(),
                        collision.entityA,
                        *setA.get<com::Damager>(),
                        tx);
        }
        engine->ecs().destroyEntity(setA.entity(), tx);
    }
}

void Damage::applyDamage(bl::ecs::Entity mortalEntity, com::Combatant& victim,
                         bl::ecs::Entity damagerEntity, com::Damager& damager, Transaction& tx) {
    bl::event::Dispatcher::dispatch<event::EntityDamaged>({mortalEntity, damagerEntity});
    if (victim.applyDamage(damager.damage)) {
        if (victim.getDeathTime() > 0.f) {
            engine->ecs().emplaceComponentWithTx<bl::com::MarkedForDeath>(
                mortalEntity, tx, victim.getDeathTime(), bl::engine::StateMask::Running);
        }
        else { engine->ecs().destroyEntity(mortalEntity, tx); }

        engine->ecs().removeComponent<com::Combatant>(mortalEntity);
        bl::event::Dispatcher::dispatch<event::EntityKilled>({mortalEntity, damagerEntity});
    }
}

} // namespace sys
} // namespace core

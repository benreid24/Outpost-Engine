#include <Core/Systems/Damage.hpp>

namespace core
{
namespace sys
{
Damage::Damage()
: engine(nullptr) {}

void Damage::init(bl::engine::Engine& e) { engine = &e; }

void Damage::makeMortal(bl::ecs::Entity entity, float health, float deathTime) {
    engine->ecs().emplaceComponent<com::Mortal>(entity, health, deathTime);
}

void Damage::makeDamager(bl::ecs::Entity entity, float damage) {
    engine->ecs().emplaceComponent<com::Damager>(entity, damage);
}

void Damage::observe(const bl::sys::Physics2D::EntityCollisionBeginEvent& collision) {
    using namespace bl::ecs;
    Transaction tx(engine->ecs());

    auto setA = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Mortal>>(
        collision.entityA, tx);
    auto setB = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Mortal>>(
        collision.entityB, tx);
    tx.unlock();

    if (setB.get<com::Damager>()) {
        if (setA.get<com::Mortal>()) {
            applyDamage(collision.entityA,
                        *setA.get<com::Mortal>(),
                        collision.entityB,
                        *setB.get<com::Damager>(),
                        tx);
        }
        engine->ecs().destroyEntity(setB.entity(), tx);
    }
    if (setA.get<com::Damager>()) {
        if (setB.get<com::Mortal>()) {
            applyDamage(collision.entityB,
                        *setB.get<com::Mortal>(),
                        collision.entityA,
                        *setA.get<com::Damager>(),
                        tx);
        }
        engine->ecs().destroyEntity(setA.entity(), tx);
    }
}

void Damage::applyDamage(bl::ecs::Entity mortalEntity, com::Mortal& victim,
                         bl::ecs::Entity damagerEntity, com::Damager& damager, Transaction& tx) {
    victim.health -= damager.damage;
    if (victim.health <= 0.f) {
        if (victim.deathTime > 0.f) {
            engine->ecs().emplaceComponentWithTx<bl::com::MarkedForDeath>(
                mortalEntity, tx, victim.deathTime, bl::engine::StateMask::Running);
        }
        else { engine->ecs().destroyEntity(mortalEntity, tx); }
    }
}

} // namespace sys
} // namespace core

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

void Damage::observe(const bl::sys::Physics2D::EntityCollisionHitEvent& collision) {
    using namespace bl::ecs;
    Transaction tx(engine->ecs());

    auto setA = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Mortal>>(
        collision.entityA, tx);
    auto setB = engine->ecs().getComponentSet<Require<>, Optional<com::Damager, com::Mortal>>(
        collision.entityB, tx);
    tx.unlock();

    if (setA.get<com::Mortal>() && setB.get<com::Damager>()) {
        applyDamage(collision.entityA,
                    *setA.get<com::Mortal>(),
                    collision.entityB,
                    *setB.get<com::Damager>(),
                    tx);
    }
    if (setB.get<com::Mortal>() && setA.get<com::Damager>()) {
        applyDamage(collision.entityB,
                    *setB.get<com::Mortal>(),
                    collision.entityA,
                    *setA.get<com::Damager>(),
                    tx);
    }
}

void Damage::applyDamage(bl::ecs::Entity mortalEntity, com::Mortal& victim,
                         bl::ecs::Entity damagerEntity, com::Damager& damager, Transaction& tx) {
    victim.health -= damager.damage;
    engine->ecs().destroyEntity(damagerEntity, tx);
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

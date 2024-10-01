#include <Core/Systems/EntityActions.hpp>

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Math.hpp>
#include <Core/Components/Damager.hpp>
#include <Core/Game.hpp>

namespace core
{
namespace sys
{
namespace
{
using BulletEcsTransaction = bl::ecs::Transaction<
    bl::ecs::tx::EntityWrite, bl::ecs::tx::ComponentRead<>,
    bl::ecs::tx::ComponentWrite<bl::com::Transform2D, bl::com::Physics2D, com::Damager,
                                bl::com::MarkedForDeath, bl::com::Hitbox2D>>;
}

EntityActions::EntityActions()
: movers(nullptr)
, shooters(nullptr) {}

void EntityActions::init(bl::engine::Engine& e) {
    engine   = &e;
    movers   = &e.ecs().getAllComponents<com::Moveable>();
    shooters = &e.ecs().getAllComponents<com::Shooter>();
}

void EntityActions::update(std::mutex&, float dt, float, float, float) {
    movers->forEach([dt](bl::ecs::Entity, com::Moveable& m) { m.apply(dt); });

    shooters->forEach([this, dt](bl::ecs::Entity entity, com::Shooter& shooter) {
        constexpr float Velocity = 60.f;
        constexpr glm::vec2 Size(2.f, 6.f);

        shooter.timeSinceLastFire += dt;
        if (shooter.isFiring) {
            shooter.isFiring = false;
            if (shooter.timeSinceLastFire >= shooter.getFireDelay()) {
                shooter.timeSinceLastFire = 0.f;

                auto& game = bl::game::Game::getInstance<Game>();
                auto& ecs  = engine->ecs();

                BulletEcsTransaction tx(ecs);
                bl::com::Transform2D* transform =
                    ecs.getComponent<bl::com::Transform2D>(entity, tx);
                if (!transform) {
                    BL_LOG_ERROR << "Tried to fire bullet from entity without transform: "
                                 << entity;
                    return;
                }

                const bl::ecs::Entity bullet =
                    ecs.createEntity(entity.getWorldIndex(), bl::ecs::Flags::WorldObject, tx);
                const glm::vec2 pos   = transform->getGlobalPosition();
                const float r         = bl::math::degreesToRadians(transform->getRotation());
                const float c         = std::cos(r);
                const float s         = std::sin(r);
                auto* bulletTransform = ecs.emplaceComponentWithTx<bl::com::Transform2D>(
                    bullet,
                    tx,
                    pos + glm::vec2(c, s) * shooter.getBulletOffset(),
                    transform->getRotation());

                ecs.emplaceComponentWithTx<com::Damager>(bullet, tx, shooter.getDamage());
                ecs.emplaceComponentWithTx<bl::com::MarkedForDeath>(
                    bullet, tx, 2.f, bl::engine::StateMask::Running);
                game.renderSystem().addTestGraphicsToEntity(bullet, Size, sf::Color::Black);

                // TODO - think about collision filters
                auto bodyDef             = b2DefaultBodyDef();
                auto shapeDef            = b2DefaultShapeDef();
                bodyDef.type             = b2_dynamicBody;
                bodyDef.fixedRotation    = true;
                bodyDef.isBullet         = true;
                bodyDef.linearVelocity.x = c * Velocity;
                bodyDef.linearVelocity.y = s * Velocity;
                bodyDef.linearDamping    = 0.f;
                ecs.emplaceComponentWithTx<bl::com::Hitbox2D>(bullet, tx, bulletTransform, Size);
                game.physicsSystem().addPhysicsToEntity(bullet, bodyDef, shapeDef);
            }
        }
    });
}

} // namespace sys
} // namespace core

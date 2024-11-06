#include <Core/Systems/Unit.hpp>

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Math.hpp>
#include <Core/Components/Damager.hpp>
#include <Core/Game.hpp>
#include <Core/World/Collisions.hpp>

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

glm::vec2 changeVectorDirection(glm::vec2 forward, unit::Moveable::MoveDirection dir) {
    switch (dir) {
    case unit::Moveable::MoveDirection::Forward:
        return forward;
    case unit::Moveable::MoveDirection::Right:
        return {-forward.y, forward.x};
    case unit::Moveable::MoveDirection::Left:
        return {forward.y, -forward.x};
    case unit::Moveable::MoveDirection::Backward:
    default:
        return -forward;
    }
}
} // namespace

void Unit::init(bl::engine::Engine& e) {
    engine = &e;
    units  = &e.ecs().getAllComponents<com::Unit>();
}

void Unit::update(std::mutex&, float dt, float, float, float) {
    processAI();
    resolveActions(dt);
}

void Unit::processAI() {
    // TODO - unit level AI
}

void Unit::resolveActions(float dt) {
    units->forEach([this, dt](bl::ecs::Entity entity, com::Unit& unit) {
        if (unit.mover.has_value()) { resolveUnitMovement(unit, dt); }
        if (unit.shooter.has_value()) { resolveUnitShoot(entity, unit, dt); }
    });
}

void Unit::resolveUnitMovement(com::Unit& unit, float dt) {
    auto& mover = unit.getMover();

    if (mover.rotateDir != unit::Moveable::NoRotate) {
        const float sign = mover.rotateDir == unit::Moveable::Clockwise ? 1.f : -1.f;
        unit.physics.getTransform().rotate(sign * mover.rotateRate * mover.rotateFactor * dt);
        mover.rotateDir = unit::Moveable::NoRotate;
    }
    if (mover.moveDir != unit::Moveable::NoMove) {
        unit.physics.setLinearDamping(0.f);
        const float radians = bl::math::degreesToRadians(unit.physics.getTransform().getRotation());
        const float c       = std::cos(radians);
        const float s       = std::sin(radians);

        const glm::vec2 vel = unit.physics.getLinearVelocity();
        const float velMag  = glm::length(vel);
        if (velMag > 0.f) {
            const glm::vec2 diff =
                (changeVectorDirection({c, s}, mover.moveDir) - glm::normalize(vel)) * velMag;
            unit.physics.applyImpulseToCenter(diff * mover.mass * mover.dirCorrectionFactor);
        }

        const glm::vec2 f = glm::vec2(c, s) * mover.force * mover.moveFactor;
        unit.physics.applyForceToCenter(changeVectorDirection(f, mover.moveDir));
        mover.moveDir = unit::Moveable::NoMove;
    }
    else { unit.physics.setLinearDamping(mover.damping); }

    if (mover.maxSpeed > 0.f) { unit.physics.clampLinearVelocity(mover.maxSpeed); }
}

void Unit::resolveUnitShoot(bl::ecs::Entity entity, com::Unit& unit, float dt) {
    constexpr float Velocity = 30.f;
    constexpr glm::vec2 Size(8.f, 3.f);

    auto& game = bl::game::Game::getInstance<Game>();
    auto& ecs  = engine->ecs();

    unit::Shooter& shooter = unit.shooter.value();
    shooter.timeSinceLastFire += dt;
    if (shooter.isFiring) {
        shooter.isFiring = false;
        if (shooter.timeSinceLastFire >= shooter.getFireDelay()) {
            shooter.timeSinceLastFire = 0.f;

            // TODO - place bullet firing somewhere else
            BulletEcsTransaction tx(ecs);
            bl::com::Transform2D* transform = ecs.getComponent<bl::com::Transform2D>(entity, tx);
            if (!transform) {
                BL_LOG_ERROR << "Tried to fire bullet from entity without transform: " << entity;
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

            auto bodyDef             = b2DefaultBodyDef();
            auto shapeDef            = b2DefaultShapeDef();
            bodyDef.type             = b2_dynamicBody;
            bodyDef.fixedRotation    = true;
            bodyDef.isBullet         = true;
            bodyDef.linearVelocity.x = c * Velocity;
            bodyDef.linearVelocity.y = s * Velocity;
            bodyDef.linearDamping    = 0.f;
            shapeDef.filter          = world::Collisions::getBulletFilter();
            ecs.emplaceComponentWithTx<bl::com::Hitbox2D>(bullet, tx, bulletTransform, Size);
            game.physicsSystem().addPhysicsToEntity(bullet, bodyDef, shapeDef);
        }
    }
}

} // namespace sys
} // namespace core

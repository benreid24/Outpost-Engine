#include <Core/Systems/Unit.hpp>

#include <BLIB/Components/Hitbox2D.hpp>
#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Math.hpp>
#include <Core/Components/Damager.hpp>
#include <Core/Game.hpp>
#include <Core/Properties.hpp>
#include <Core/World/Collisions.hpp>
#include <cmath>

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

glm::vec2 changeVectorDirection(glm::vec2 forward, unit::able::Move::MoveDirection dir) {
    switch (dir) {
    case unit::able::Move::MoveDirection::Forward:
        return forward;
    case unit::able::Move::MoveDirection::Right:
        return {-forward.y, forward.x};
    case unit::able::Move::MoveDirection::Left:
        return {forward.y, -forward.x};
    case unit::able::Move::MoveDirection::Backward:
    default:
        return -forward;
    }
}

unit::able::Rotate::RotateDirection getRotateDirection(float angle, float target) {
    auto dir       = unit::able::Rotate::Clockwise;
    float opposite = angle + 180.f;
    if (opposite >= 360.f) {
        opposite -= 360.f;
        if (target < angle && target > opposite) { dir = unit::able::Rotate::CounterClockwise; }
    }
    else {
        if (target < angle || target > opposite) { dir = unit::able::Rotate::CounterClockwise; }
    }
    return dir;
}

} // namespace

void Unit::init(bl::engine::Engine& e) {
    engine = &e;
    units  = &e.ecs().getAllComponents<com::Unit>();
}

void Unit::update(std::mutex&, float dt, float, float, float) { doCapabilities(dt); }

void Unit::doCapabilities(float dt) {
    units->forEach([this, dt](bl::ecs::Entity entity, com::Unit& unit) {
        auto* rotater = unit.capabilities().get<unit::Capability::Rotate>();
        if (rotater && rotater->rotateDir != unit::able::Rotate::NoRotate) {
            using Dir        = unit::able::Rotate::RotateDirection;
            const float sign = rotater->rotateDir == Dir::Clockwise ? 1.f : -1.f;
            unit.physics.getTransform().rotate(sign * rotater->rotateRate * rotater->rotateFactor *
                                               dt);
            rotater->rotateDir = Dir::NoRotate;
        }

        auto* mover = unit.capabilities().get<unit::Capability::Move>();
        if (mover) {
            using Dir = unit::able::Move::MoveDirection;

            if (mover->moveDir != Dir::NoMove) {
                unit.physics.setLinearDamping(0.f);
                const float mass = unit.physics.getMass();
                const float radians =
                    bl::math::degreesToRadians(unit.physics.getTransform().getRotation());
                const float c = std::cos(radians);
                const float s = std::sin(radians);

                const glm::vec2 vel = unit.physics.getLinearVelocity();
                const float velMag  = glm::length(vel);
                if (velMag > 0.f) {
                    const glm::vec2 diff =
                        (changeVectorDirection({c, s}, mover->moveDir) - glm::normalize(vel)) *
                        velMag;
                    unit.physics.applyImpulseToCenter(diff * mass *
                                                      mover->directionCorrectionFactor);
                }

                const glm::vec2 f =
                    glm::vec2(c, s) * (mover->acceleration * mass) * mover->moveFactor;
                unit.physics.applyForceToCenter(changeVectorDirection(f, mover->moveDir));
                mover->moveDir = Dir::NoMove;
            }
            else { unit.physics.setLinearDamping(mover->damping); }

            if (mover->maxSpeed > 0.f) { unit.physics.clampLinearVelocity(mover->maxSpeed); }
        }

        auto* shooter = unit.capabilities().get<unit::Capability::Shoot>();
        if (shooter) {
            constexpr float Velocity = 30.f;
            constexpr glm::vec2 Size(8.f, 3.f);

            shooter->timeSinceLastFire += dt;
            if (shooter->isFiring) {
                shooter->isFiring = false;
                if (shooter->timeSinceLastFire >= shooter->fireDelay) {
                    shooter->timeSinceLastFire = 0.f;

                    auto& game = bl::game::Game::getInstance<Game>();
                    auto& ecs  = engine->ecs();

                    // TODO - place bullet firing somewhere else
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
                        pos + glm::vec2(c, s) * shooter->bulletOffset,
                        transform->getRotation());

                    ecs.emplaceComponentWithTx<com::Damager>(bullet, tx, shooter->damage);
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
                    ecs.emplaceComponentWithTx<bl::com::Hitbox2D>(
                        bullet, tx, bulletTransform, Size);
                    game.physicsSystem().addPhysicsToEntity(bullet, bodyDef, shapeDef);
                }
            }
        }
    });
}

} // namespace sys
} // namespace core

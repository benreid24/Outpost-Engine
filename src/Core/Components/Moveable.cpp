#include <Core/Components/Moveable.hpp>

#include <BLIB/Math.hpp>

namespace core
{
namespace com
{
namespace
{
glm::vec2 changeVectorDirection(glm::vec2 forward, Moveable::MoveDirection dir) {
    switch (dir) {
    case Moveable::MoveDirection::Forward:
        return forward;
    case Moveable::MoveDirection::Right:
        return {-forward.y, forward.x};
    case Moveable::MoveDirection::Left:
        return {forward.y, -forward.x};
    case Moveable::MoveDirection::Backward:
    default:
        return -forward;
    }
}
} // namespace

Moveable::Moveable(bl::com::Physics2D& physics, float accel, float maxVel, float rotate,
                   float dirFactor, float damp)
: physics(physics)
, mass(physics.getMass())
, maxSpeed(maxVel)
, rotateRate(rotate)
, dirCorrectionFactor(dirFactor)
, damping(damp)
, force(physics.scaleWorldToPhysics(accel) * mass)
, moveDir(NoMove)
, moveFactor(1.f)
, rotateDir(NoRotate)
, rotateFactor(1.f) {}

void Moveable::move(MoveDirection direction, float factor) {
    moveDir    = direction;
    moveFactor = factor;
}

void Moveable::rotate(RotateDirection direction, float factor) {
    rotateDir    = direction;
    rotateFactor = factor;
}

void Moveable::setRotation(float degrees) { physics.getTransform().setRotation(degrees); }

void Moveable::apply(float dt) {
    if (rotateDir != NoRotate) {
        const float sign = rotateDir == Clockwise ? 1.f : -1.f;
        physics.getTransform().rotate(sign * rotateRate * rotateFactor * dt);
        rotateDir = NoRotate;
    }
    if (moveDir != NoMove) {
        physics.setLinearDamping(0.f);
        const float radians = bl::math::degreesToRadians(physics.getTransform().getRotation());
        const float c       = std::cos(radians);
        const float s       = std::sin(radians);

        const glm::vec2 vel = physics.getLinearVelocity();
        const float velMag  = glm::length(vel);
        if (velMag > 0.f) {
            const glm::vec2 diff =
                (changeVectorDirection({c, s}, moveDir) - glm::normalize(vel)) * velMag;
            physics.applyImpulseToCenter(diff * mass * dirCorrectionFactor);
        }

        const glm::vec2 f = glm::vec2(c, s) * force * moveFactor;
        physics.applyForceToCenter(changeVectorDirection(f, moveDir));
        moveDir = NoMove;
    }
    else { physics.setLinearDamping(damping); }

    if (maxSpeed > 0.f) { physics.clampLinearVelocity(maxSpeed); }
}

} // namespace com
} // namespace core

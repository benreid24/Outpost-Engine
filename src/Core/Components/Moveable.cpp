#include <Core/Components/Moveable.hpp>

#include <BLIB/Math.hpp>

namespace core
{
namespace com
{
Moveable::Moveable(bl::com::Physics2D& physics, float accel, float rotate, float maxVel)
: physics(physics)
, maxSpeed(maxVel)
, rotateRate(rotate)
, force(physics.scaleWorldToPhysics(accel) * physics.getMass())
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
        const glm::vec2 vec = glm::vec2(c, s) * force * moveFactor;
        switch (moveDir) {
        case Forward:
            physics.applyForceToCenter(vec);
            break;
        case Right:
            physics.applyForceToCenter({-vec.y, vec.x});
            break;
        case Left:
            physics.applyForceToCenter({vec.y, -vec.x});
            break;
        case Backward:
        default:
            physics.applyForceToCenter(-vec);
        }
        moveDir = NoMove;
    }
    else { physics.setLinearDamping(8.f); }

    if (maxSpeed > 0.f) { physics.clampLinearVelocity(maxSpeed); }
}

} // namespace com
} // namespace core

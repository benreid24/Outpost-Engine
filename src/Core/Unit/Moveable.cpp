#include <Core/Unit/Moveable.hpp>

#include <BLIB/Math.hpp>

namespace core
{
namespace unit
{
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

} // namespace unit
} // namespace core

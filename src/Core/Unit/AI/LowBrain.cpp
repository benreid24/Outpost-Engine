#include <Core/Unit/AI/LowBrain.hpp>

#include <BLIB/Math.hpp>
#include <Core/Components/Unit.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace unit
{
namespace ai
{
LowBrain::LowBrain(com::Unit& owner)
: owner(owner)
, state(Idle) {}

void LowBrain::update(const UpdateContext& ctx) {
    switch (state) {
    case Moving:
        processMove(ctx.dt);
        break;

    case Firing:
        processFire(ctx.dt);
        break;

    default:
        break;
    }
}

void LowBrain::notify(const Notification& n) {
    switch (state) {
    case Firing:
        switch (n.type) {
        case Notification::KilledTarget:
            if (n.killed == killTarget.entity) { state = Done; }
            break;

        case Notification::DamageGiven:
            // TODO - track if we aren't hitting?
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void LowBrain::makeIdle() { state = Done; }

void LowBrain::moveToPosition(const glm::vec2& pos) {
    state                   = Moving;
    movement.targetPosition = pos;
}

void LowBrain::fireAtTarget(com::Combatant* target) {
    state             = Firing;
    killTarget.entity = target->getId();
    killTarget.target = target;
}

void LowBrain::processMove(float dt) {
    auto* mover   = owner.capabilities().get<Capability::Move>();
    auto* rotater = owner.capabilities().get<Capability::Rotate>();
    if (!mover || !rotater) {
        state = Failed;
        return;
    }

    const glm::vec2 pos     = owner.getPosition();
    const float distance    = glm::distance(pos, movement.targetPosition);
    const float targetAngle = bl::math::computeAngle(pos, movement.targetPosition);

    // point towards position
    const float angle     = owner.getRotation();
    const float angleDiff = std::abs(angle - targetAngle);
    if (angleDiff > 1.f) {
        rotate(targetAngle, dt);
        if (distance / mover->maxSpeed < angleDiff / rotater->rotateRate * 0.4f) { return; }
    }

    // check if done
    if (distance < Properties.UnitAiDistanceStopThresh.get()) {
        state = Done;
        return;
    }

    // move towards position
    mover->move(able::Move::Forward, 1.f - angleDiff / 180.f);
}

void LowBrain::processFire(float dt) {
    auto* shooter = owner.capabilities().get<Capability::Shoot>();
    auto* rotater = owner.capabilities().get<Capability::Rotate>();
    if (!shooter || !rotater) {
        state = Failed;
        return;
    }

    // point towards target and fire if close enough
    const float angle = owner.getRotation();
    const float targetAngle =
        bl::math::computeAngle(owner.getPosition(), killTarget.target->getPosition());
    if (std::abs(angle - targetAngle) > 1.f) { rotate(targetAngle, dt); }
    else { shooter->fire(); }
}

void LowBrain::rotate(float target, float dt) {
    auto* rotater     = owner.capabilities().get<Capability::Rotate>();
    auto dir          = able::Rotate::Clockwise;
    const float angle = owner.getRotation();
    float opposite    = angle + 180.f;
    if (opposite >= 360.f) {
        opposite -= 360.f;
        if (target < angle && target > opposite) { dir = able::Rotate::CounterClockwise; }
    }
    else {
        if (target < angle || target > opposite) { dir = able::Rotate::CounterClockwise; }
    }

    const float maxRot = rotater->rotateRate * dt;
    const float diff   = std::abs(angle - target);
    const float factor = diff >= maxRot ? 1.f : diff / maxRot;
    rotater->rotate(dir, factor);
}

} // namespace ai
} // namespace unit
} // namespace core

#include <Core/Unit/AI/MidBrain.hpp>

#include <Core/Components/Unit.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace unit
{
namespace ai
{
MidBrain::MidBrain(com::Unit& owner, LowBrain& lowBrain)
: owner(owner)
, lowBrain(lowBrain)
, state(Idle)
, target(nullptr)
, waitingForLowBrain(false) {}

void MidBrain::update(const UpdateContext& ctx) {
    switch (state) {
    case MovingToPosition:
    case MovingToKill:
        processMovement(ctx);
        break;

    case ShootingToKill:
        processShooting(ctx);
        break;

    default:
        break;
    }
}

void MidBrain::notify(const Notification& n) {
    switch (state) {
    case MovingToKill:
    case ShootingToKill:
        if (n.type == Notification::KilledTarget) {
            if (n.killed == targetEntity) { enterState(Done); }
        }
        break;

    default:
        break;
    }
}

void MidBrain::makeIdle() {
    enterState(Done);
    lowBrain.makeIdle();
}

void MidBrain::attackTarget(com::Combatant* t) {
    enterState(ShootingToKill);
    targetEntity = t->getId();
    target       = t;
}

void MidBrain::moveToPosition(const glm::vec2& pos) {
    enterState(MovingToPosition);
    destPos = pos;
}

void MidBrain::enterState(State newState) {
    state = newState;
    path.waypoints.clear();
    waitingForLowBrain = false;
}

void MidBrain::processMovement(const UpdateContext& ctx) {
    // ensure we have a path
    if (path.waypoints.empty()) {
        if (!ctx.world.computePath(owner.getPosition(), destPos, path)) {
            enterState(Failed);
            return;
        }
    }

    const auto& wp = path.waypoints[path.currentWaypoint];

    // check if done or close enough
    if (&wp == &path.waypoints.back()) {
        if (glm::distance(owner.getPosition(), wp.position) <
            Properties.UnitAiDistanceStopThresh.get()) {
            if (state == MovingToPosition) { enterState(Done); }
            else { enterState(ShootingToKill); }
            return;
        }
    }

    if (!waitingForLowBrain) {
        const bool occupied = wp.worldNode &&
                              wp.worldNode->getOccupier() != bl::ecs::InvalidEntity &&
                              wp.worldNode->getOccupier() != owner.getId();
        if (!occupied && ctx.world.pathToPositionIsClear(owner.getPosition(), wp.position)) {
            lowBrain.moveToPosition(wp.position);
            waitingForLowBrain = true;
        }
        else { path.waypoints.clear(); }
    }
    else {
        // check low brain state
        switch (lowBrain.getState()) {
        case LowBrain::Moving:
            // noop, wait
            break;

        case LowBrain::Done:
            if (&wp == &path.waypoints.back()) {
                if (state == MovingToPosition) { enterState(Done); }
                else { enterState(ShootingToKill); }
            }
            else {
                ++path.currentWaypoint;
                waitingForLowBrain = false;
            }
            break;

        case LowBrain::Failed:
            waitingForLowBrain = false;
            path.waypoints.clear();
            break;

        default:
            // we shouldn't get here
            BL_LOG_WARN << "Unit low brain in unexpected state: " << lowBrain.getState();
            waitingForLowBrain = false;
            break;
        }
    }
}

void MidBrain::processShooting(const UpdateContext& ctx) {
    const auto pickFiringPosition = [this, &ctx]() {
        const world::Node* node = ctx.world.pickFiringPosition(
            owner.getPosition(), target->getPosition(), Properties.UnitAiFiringMaxDistance.get());
        if (!node) {
            BL_LOG_WARN << "Unit could not find suitable firing position";
            enterState(Failed);
            return;
        }
        destPos = node->getPosition();
        enterState(MovingToKill);
    };

    // check if we have a line and are in range
    if (!waitingForLowBrain) {
        const float distance = glm::distance(owner.getPosition(), target->getPosition());
        if (distance >= Properties.UnitAiFiringMaxDistance.get()) {
            pickFiringPosition();
            return;
        }

        com::Combatant* see = ctx.world.lineOfSightIsClear(owner.getPosition(), target);
        if (see == target || see->getFaction() != owner.getFaction()) {
            lowBrain.fireAtTarget(target);
            waitingForLowBrain = true;
            waitingTime        = 0.f;
            return;
        }
        else {
            pickFiringPosition();
            return;
        }
    }
    else {
        switch (lowBrain.getState()) {
        case LowBrain::Firing:
            waitingTime += ctx.dt;
            if (waitingTime >= Properties.UnitAiFiringLineRecheckTime.get()) {
                waitingForLowBrain = false;
            }
            break;

        case LowBrain::Done:
            enterState(Done);
            break;

        case LowBrain::Failed:
            enterState(Failed);
            break;

        default:
            BL_LOG_WARN << "Unit low brain in unexpected state: " << lowBrain.getState();
            waitingForLowBrain = false;
            break;
        }
    }
}

} // namespace ai
} // namespace unit
} // namespace core

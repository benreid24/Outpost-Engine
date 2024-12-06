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

void MidBrain::attackTarget(com::Combatant* t) {
    enterState(MovingToKill);
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
    //
}

} // namespace ai
} // namespace unit
} // namespace core

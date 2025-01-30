#include <Core/Unit/AI/TopBrain.hpp>

namespace core
{
namespace unit
{
namespace ai
{
TopBrain::TopBrain(com::Unit& owner, MidBrain& mb)
: owner(owner)
, midBrain(mb)
, commandState(CommandState::Done)
, state(Idle)
, stance(cmd::AggroLevel::Neutral) {}

void TopBrain::update(const UpdateContext& ctx) {
    switch (state) {
    case IdlePursuing:
        // TODO - chase & attack target
        break;

    case IdleRetreating:
        // TODO - move away and check safety
        break;

    case Moving:
        checkMidBrainResult();
        break;

    case Attacking:
        checkMidBrainResult();
        break;

    case Suppressing:
        // TODO - implement suppression
        break;

    case SuppressPursuing:
        // TODO - implement suppression
        break;

    case SuppressRetreating:
        // TODO - implement suppression
        break;

    case Idle:
    default:
        break;
    }
}

void TopBrain::notify(const Notification& notification) {
    // TODO - process (like retreat if shot etc)
}

void TopBrain::issueCommand(const cmd::UnitCommand& command) {
    commandState = CommandState::Executing;
    stance       = command.getAggroLevel();

    using T = cmd::UnitCommand::Type;
    switch (command.getType()) {
    case T::Idle:
        state = Idle;
        break;

    case T::Attack:
        state = Attacking;
        midBrain.attackTarget(command.getTarget());
        break;

    case T::Move:
        state = Moving;
        midBrain.moveToPosition(command.getTargetPosition());
        break;

    case T::Suppress:
        // TODO - implement suppression
        break;

    default:
        break;
    }
}

void TopBrain::makeIdle(CommandState cs) {
    state        = Idle;
    commandState = cs;
    midBrain.makeIdle();
}

void TopBrain::checkMidBrainResult() {
    switch (midBrain.getState()) {
    case MidBrain::Done:
        makeIdle(CommandState::Done);
        break;
    case MidBrain::Failed:
        makeIdle(CommandState::Failed);
        break;
    default:
        break;
    }
}

} // namespace ai
} // namespace unit
} // namespace core

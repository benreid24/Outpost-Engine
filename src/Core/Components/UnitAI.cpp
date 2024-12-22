#include <Core/Components/UnitAI.hpp>

namespace core
{
namespace com
{
UnitAI::UnitAI(com::Unit& owner)
: owner(owner)
, topBrain(owner, midBrain)
, midBrain(owner, lowBrain)
, lowBrain(owner) {}

void UnitAI::update(const unit::ai::UpdateContext& ctx) {
    if (commandQueue.hasCommand()) {
        using State           = unit::ai::TopBrain::CommandState;
        bool issueNextCommand = false;
        switch (topBrain.getCommandState()) {
        case State::Done:
            commandQueue.completeCurrentCommand();
            issueNextCommand = true;
            break;
        case State::Failed:
            commandQueue.failCurrentCommand();
            issueNextCommand = true;
            break;
        case State::Executing:
        default:
            break;
        }

        if (issueNextCommand && commandQueue.hasCommand()) {
            topBrain.issueCommand(commandQueue.getCurrentCommand());
        }
    }

    topBrain.update(ctx);
    midBrain.update(ctx);
    lowBrain.update(ctx);
}

void UnitAI::notify(const unit::ai::Notification& n) {
    lowBrain.notify(n);
    midBrain.notify(n);
    topBrain.notify(n);
}

void UnitAI::queueCommand(const cmd::UnitCommandHandle& cmd, cmd::AddMode addMode) {
    auto local = cmd;
    if (commandQueue.queueCommand(std::move(local), addMode)) {
        topBrain.issueCommand(commandQueue.getCurrentCommand());
    }
}

void UnitAI::startNextCommand() {
    topBrain.makeIdle();
    topBrain.issueCommand(commandQueue.getCurrentCommand());
}

} // namespace com
} // namespace core

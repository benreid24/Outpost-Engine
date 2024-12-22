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
    // TODO - command queue management

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
    commandQueue.queueCommand(std::move(local), addMode);
}

} // namespace com
} // namespace core

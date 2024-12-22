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
, state(Idle)
, stance(cmd::AggroLevel::Neutral) {}

void TopBrain::update(const UpdateContext& ctx) {
    // TODO - high level ai update
}

void TopBrain::notify(const Notification& notification) {
    // TODO - process (like retreat if shot etc)
}

void TopBrain::issueCommand(const cmd::UnitCommand& command) {
    // TODO - update state based on command
}

} // namespace ai
} // namespace unit
} // namespace core

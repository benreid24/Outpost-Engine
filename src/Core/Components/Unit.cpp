#include <Core/Components/Unit.hpp>

namespace core
{
namespace com
{
Unit::Unit(fcn::FactionId faction, bl::com::Physics2D& physics)
: faction(faction)
, physics(physics) {}

void Unit::queueCommand(const cmd::UnitCommandHandle& cmd, cmd::AddMode addMode) {
    auto cmdCopy = cmd;
    commandQueue.queueCommand(std::move(cmdCopy), addMode);
}

} // namespace com
} // namespace core

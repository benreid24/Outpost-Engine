#include <Core/Components/Unit.hpp>

namespace core
{
namespace com
{
Unit::Unit(fcn::FactionId faction, bl::com::Physics2D& physics)
: faction(faction)
, physics(physics) {}

void Unit::makeMoveable(float acceleration, float maxSpeed, float rotateRate,
                        float directionAdjustSpeed, float damping) {
    mover.emplace(physics, acceleration, maxSpeed, rotateRate, directionAdjustSpeed, damping);
}

void Unit::makeShooter(float fireRate, float damage, float bulletOffset) {
    shooter.emplace(fireRate, damage, bulletOffset);
}

bool Unit::queueCommand(const cmd::ExternalHandle<cmd::UnitCommand>& cmd) {
    if (!queuedCommands.full()) {
        queuedCommands.emplace_back(makeCommandHandle(cmd));
        queuedCommands.back().markQueued();
        return true;
    }
    return false;
}

} // namespace com
} // namespace core

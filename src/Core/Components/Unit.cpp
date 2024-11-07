#include <Core/Components/Unit.hpp>

namespace core
{
namespace com
{
Unit::Unit(bl::com::Physics2D& physics)
: physics(physics) {}

void Unit::makeMoveable(float acceleration, float maxSpeed, float rotateRate,
                        float directionAdjustSpeed, float damping) {
    mover.emplace(physics, acceleration, maxSpeed, rotateRate, directionAdjustSpeed, damping);
}

void Unit::makeShooter(float fireRate, float damage, float bulletOffset) {
    shooter.emplace(fireRate, damage, bulletOffset);
}

} // namespace com
} // namespace core
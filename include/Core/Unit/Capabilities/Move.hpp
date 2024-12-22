#ifndef CORE_UNIT_CAPABILITIES_MOVE_HPP
#define CORE_UNIT_CAPABILITIES_MOVE_HPP

#include <Core/Properties.hpp>

namespace core
{
namespace sys
{
class Unit;
}

namespace unit
{
/// Namespace for info structs for unit capabilities
namespace able
{
/**
 * @brief Capability that allows a unit to be moved
 *
 * @ingroup Unit
 */
class Move {
public:
    /// The direction to move in, relative to the direction the entity is facing
    enum MoveDirection { NoMove, Forward, Right, Left, Backward };

    /**
     * @brief Creates the movement capability with default values
     */
    Move()
    : acceleration(Properties.UnitAccelerationDefault.get())
    , maxSpeed(Properties.UnitMaxSpeedDefault.get())
    , directionCorrectionFactor(Properties.UnitDirCorrectFactorDefault.get())
    , damping(Properties.UnitDampingFactor.get() * maxSpeed)
    , moveDir(NoMove)
    , moveFactor(1.f)
    , slowdownStart(-1.f) {}

    /**
     * @brief Moves the entity in the given direction. Only the last call to this method in a frame
     *        is processed
     *
     * @param direction The direction to move in
     * @param factor A scale factor to apply to the acceleration of the entity
     */
    void move(MoveDirection direction = Forward, float factor = 1.f) {
        moveDir    = direction;
        moveFactor = factor;
    }

    /// Acceleration of the unit in world units
    float acceleration;

    /// Max speed of the unit in world units
    float maxSpeed;

    /// Multiplicative factor for travel direction adjustment when turning while moving
    float directionCorrectionFactor;

    /// Linear damping for stopping the unit when not moving
    float damping;

private:
    MoveDirection moveDir;
    float moveFactor;
    float slowdownStart;

    friend class sys::Unit;
};

} // namespace able
} // namespace unit
} // namespace core

#endif

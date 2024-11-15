#ifndef CORE_UNIT_MOVEABLE_HPP
#define CORE_UNIT_MOVEABLE_HPP

#include <BLIB/Components/Physics2D.hpp>

namespace core
{
namespace sys
{
class Unit;
}

namespace unit
{
/**
 * @brief Component that allows a unit to be moved
 *
 * @ingroup Unit
 */
class Moveable {
public:
    /// The direction to move in, relative to the direction the entity is facing
    enum MoveDirection { NoMove, Forward, Right, Left, Backward };

    /// The direction to rotate
    enum RotateDirection { NoRotate, Clockwise, CounterClockwise };

    /**
     * @brief Creates the moveable component
     *
     * @param physics The physics component of the unit
     * @param acceleration The rate of acceleration for movement, in world space units
     * @param maxSpeed The maximum movement speed to enforce, in world space units
     * @param rotateRate The rotation speed in degrees per second
     * @param directionAdjustSpeed How quickly the entity velocity changes direction. In range [0,1]
     * @param damping The damping factor to use to stop the entity when not moving
     */
    Moveable(bl::com::Physics2D& physics, float acceleration, float maxSpeed, float rotateRate,
             float directionAdjustSpeed, float damping = -1.f);

    /**
     * @brief Moves the entity in the given direction. Only the last call to this method in a frame
     *        is processed
     *
     * @param direction The direction to move in
     * @param factor A scale factor to apply to the acceleration of the entity
     */
    void move(MoveDirection direction = Forward, float factor = 1.f);

    /**
     * @brief Rotates the entity in the given direction. Only the last call to this method in a
     *        frame is processed
     *
     * @param direction The direction to rotate in
     * @param factor A scale factor to apply to the rotation speed of the entity
     */
    void rotate(RotateDirection direction, float factor = 1.f);

    /**
     * @brief Sets the rotation angle of the entity
     *
     * @param degrees The angle, in degrees
     */
    void setRotation(float degrees);

private:
    bl::com::Physics2D& physics;
    const float mass;
    const float maxSpeed;
    const float rotateRate;
    const float force;
    const float dirCorrectionFactor;
    const float damping;
    MoveDirection moveDir;
    float moveFactor;
    float slowdownStart;
    RotateDirection rotateDir;
    float rotateFactor;

    friend class sys::Unit;
};

} // namespace unit
} // namespace core

#endif

#ifndef CORE_COMPONENTS_MOVEABLE_HPP
#define CORE_COMPONENTS_MOVEABLE_HPP

#include <BLIB/Components/Physics2D.hpp>

namespace core
{
namespace sys
{
class Movement;
}

namespace com
{
/**
 * @brief Component that allows an entity to be moved and controlled
 *
 * @ingroup Components
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
     * @param physics The physics component of the entity to move
     * @param acceleration The rate of acceleration for movement, in world space units
     * @param rotateRate The rotation speed in degrees per second
     * @param maxSpeed The maximum movement speed to enforce
     */
    Moveable(bl::com::Physics2D& physics, float acceleration, float rotateRate,
             float maxSpeed = 0.f);

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
    const float maxSpeed;
    const float rotateRate;
    const float force;
    bl::com::Physics2D& physics;
    MoveDirection moveDir;
    float moveFactor;
    RotateDirection rotateDir;
    float rotateFactor;

    void apply(float dt);

    friend class sys::Movement;
};

} // namespace com
} // namespace core

#endif

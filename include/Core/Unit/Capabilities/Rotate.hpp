#ifndef CORE_UNIT_CAPABILITIES_ROTATE_HPP
#define CORE_UNIT_CAPABILITIES_ROTATE_HPP

#include <Core/Properties.hpp>

namespace core
{
namespace sys
{
class Unit;
}

namespace unit
{
namespace able
{
class Rotate {
public:
    /// The direction to rotate
    enum RotateDirection { NoRotate, Clockwise, CounterClockwise };

    /**
     * @brief Creates the rotate capability with default values
     */
    Rotate()
    : rotateRate(Properties.UnitRotateRateDefault.get())
    , rotateDir(NoRotate)
    , rotateFactor(1.f) {}

    /**
     * @brief Rotates the entity in the given direction. Only the last call to this method in a
     *        frame is processed
     *
     * @param direction The direction to rotate in
     * @param factor A scale factor to apply to the rotation speed of the entity
     */
    void rotate(RotateDirection direction, float factor = 1.f) {
        rotateDir    = direction;
        rotateFactor = factor;
    }

    /// Unit rotation rate in degrees per second
    float rotateRate;

private:
    RotateDirection rotateDir;
    float rotateFactor;

    friend class sys::Unit;
};

} // namespace able
} // namespace unit
} // namespace core

#endif

#ifndef CORE_UNIT_AI_MIDBRAIN_HPP
#define CORE_UNIT_AI_MIDBRAIN_HPP

#include <Core/Unit/AI/LowBrain.hpp>
#include <Core/World/Path.hpp>
#include <variant>

namespace core
{
namespace com
{
class Unit;
}
namespace unit
{
namespace ai
{
class MidBrain {
public:
    enum State { Idle, MovingToPosition, MovingToKill, ShootingToKill, Done = Idle, Failed };

    MidBrain(com::Unit& owner, LowBrain& lowBrain);

    //

private:
    com::Unit& owner;
    LowBrain& lowBrain;
    State state;

    world::Path path;
    com::Unit* target; // TODO - target base
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

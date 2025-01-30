#ifndef CORE_UNIT_AI_UPDATECONTEXT_HPP
#define CORE_UNIT_AI_UPDATECONTEXT_HPP

namespace core
{
class Game;
namespace world
{
class World;
}

namespace unit
{
namespace ai
{
/**
 * @brief Simple struct containing parameters for unit AI updates
 *
 * @ingroup Unit
 */
struct UpdateContext {
    Game& game;
    world::World& world;
    float dt;
};

} // namespace ai
} // namespace unit
} // namespace core

#endif

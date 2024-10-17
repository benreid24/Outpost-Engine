#ifndef CORE_COMPONENTS_WORLDNODE_HPP
#define CORE_COMPONENTS_WORLDNODE_HPP

#include <cstdint>

namespace core
{
namespace world
{
class World;
} // namespace world

namespace com
{
/**
 * @brief Basic component that links an entity to a node in the world
 *
 * @ingroup Components
 */
struct WorldNode {
    world::World* world;
    std::size_t node;
};

} // namespace com
} // namespace core

#endif

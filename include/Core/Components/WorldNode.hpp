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

    /**
     * @brief Creates an empty node component
     */
    WorldNode()
    : world(nullptr)
    , node(0) {}

    /**
     * @brief Creates the node component for the given world and index
     *
     * @param world The world the node belongs to
     * @param node The index of the node in the world
     */
    WorldNode(world::World* world, std::size_t node)
    : world(world)
    , node(node) {}
};

} // namespace com
} // namespace core

#endif

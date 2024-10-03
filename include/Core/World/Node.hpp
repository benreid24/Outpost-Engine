#ifndef CORE_WORLD_NODE_HPP
#define CORE_WORLD_NODE_HPP

#include <BLIB/ECS/Entity.hpp>

namespace core
{
namespace world
{
class World;

/**
 * @brief Spatial node in a world. Nodes are used for pathing and cover
 *
 * @ingroup World
 */
class Node {
public:
    enum Type { Path, Cover };

    // TODO - interface

private:
    Type type;
    float distanceToCover[12];
    bl::ecs::Entity occupiedBy;
    bl::ecs::Entity targetedBy;

    friend class World;
};

} // namespace world
} // namespace core

#endif

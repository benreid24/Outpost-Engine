#ifndef CORE_WORLD_NODE_HPP
#define CORE_WORLD_NODE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <glm/glm.hpp>
#include <vector>

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
    /// The maximum distance to cover that is queried, in world space
    static constexpr float MaxCoverDistance = 175.f;

    /// The types of nodes
    enum Type { Path, Cover };

    /**
     * @brief Constructs the node from a type and position
     *
     * @param type The type of node to be
     * @param pos The position of the node in world space
     */
    Node(Type type, glm::vec2 pos);

    /**
     * @brief Returns the type of node this is
     */
    Type getType() const { return type; }

    /**
     * @brief Returns the distance to the nearest cover from this node facing the given angle
     *
     * @param degrees The angle to query, in degrees
     * @return The distance to the nearest cover, or MaxCoverDistance if not near cover
     */
    float getDistanceToCover(float degrees) const;

    /**
     * @brief Returns the position of the node in world space
     */
    const glm::vec2& getPosition() const { return position; }

private:
    Type type;
    glm::vec2 position;
    float distanceToCover[12];
    bl::ecs::Entity occupiedBy;
    bl::ecs::Entity targetedBy;
    std::vector<std::uint32_t> connectsTo;
    bl::ecs::Entity sensorEntity;

    friend class World;
};

} // namespace world
} // namespace core

#endif

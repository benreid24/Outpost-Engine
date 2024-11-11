#ifndef CORE_WORLD_WORLD_HPP
#define CORE_WORLD_WORLD_HPP

#include <BLIB/Engine/Worlds/World2D.hpp>
#include <BLIB/Graphics/VertexBuffer2D.hpp>
#include <Core/Components/Unit.hpp>
#include <Core/World/Cover.hpp>
#include <Core/World/Node.hpp>
#include <list>
#include <vector>

/**
 * @addtogroup World
 * @ingroup Core
 * @brief Collection of classes for the in-game world
 */

namespace core
{
/// Collection of classes for the in-game world
namespace world
{
/**
 * @brief The game world class
 *
 * @ingroup World
 */
class World : public bl::engine::World2D {
public:
    /**
     * @brief Creates the world
     *
     * @param engine The game engine instance
     */
    World(bl::engine::Engine& engine);

    /**
     * @brief Destroys the world
     */
    virtual ~World() = default;

    /**
     * @brief Adds a node to the world
     *
     * @param type The type of node to add
     * @param position The position to add the node at
     */
    void addNode(Node::Type type, glm::vec2 position);

    /**
     * @brief Removes the node closest to the given position
     *
     * @param pos The world position to query and remove from
     * @param threshold Minimum distance a node must be within to be removed
     * @return Whether a node was removed or not
     */
    bool removeNodeAtPosition(const glm::vec2& pos, float threshold = 30.f);

    /**
     * @brief Adds a section of cover (as a rotated rectangle) to the world
     *
     * @param pos The position to add the cover at. Center of the rectangle
     * @param size The side of the cover
     * @param angle The rotation of the cover in degrees
     */
    void addCover(glm::vec2 pos, glm::vec2 size, float angle);

    /**
     * @brief Removes the cover that contains the given position, if any
     *
     * @param pos The position to remove the cover at
     * @return Whether a cover was removed or not
     */
    bool removeCoverAtPosition(const glm::vec2& pos);

    /**
     * @brief Called when sensors are triggered by entities
     *
     * @param node The index of the node whose sensor was triggered
     * @param entity The entity that triggered the sensor
     */
    void handleSensorEnter(std::size_t node, bl::ecs::Entity entity);

    /**
     * @brief Called when sensors are triggered by entities
     *
     * @param node The index of the node whose sensor was triggered
     * @param entity The entity that triggered the sensor
     */
    void handleSensorExit(std::size_t node, bl::ecs::Entity entity);

    /**
     * @brief Returns the closest reachable node to the given position
     *
     * @param position The position to get the closest node of
     * @param secondaryPosition The next position to try to go to. Pass same value to ignore
     * @return The closest reachable node to the given position. May be nullptr
     */
    const Node* getClosestReachableNode(const glm::vec2& position,
                                        const glm::vec2& secondaryPosition) const;

    /**
     * @brief Finds a path from the start position to the target node
     *
     * @param startPos The start position in world coordinates
     * @param target The target node
     * @param path Vector to store the path of nodes in
     * @return Whether a path could be found or not
     */
    bool computePath(const glm::vec2& startPos, const Node* target, std::vector<const Node*>& path);

    /**
     * @brief Returns the unit at the given world position
     *
     * @param worldPos The world position to query
     * @return The unit at the position. May be nullptr
     */
    com::Unit* getUnitAtPosition(const glm::vec2& worldPos) const;

private:
    std::vector<Cover> covers;
    std::vector<Node> nodes;
    // TODO - spatial partitions?

    bool pathToNodeIsClear(const glm::vec2& pos, const Node& node) const;

    // debug data
    bl::gfx::VertexBuffer2D debugNodes;
    bl::gfx::VertexBuffer2D debugNodeEdges;

    void addDebugGraphicsToNode(Node& node);
    void recomputeNodeDistances();
    void computeNodeDistances(Node& node);
    void populateNodeEdges(Node& node);
    void repopulateAllNodeEdges();
    void regenNodeGraphics();
};

} // namespace world
} // namespace core

#endif

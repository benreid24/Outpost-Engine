#ifndef CORE_WORLD_WORLD_HPP
#define CORE_WORLD_WORLD_HPP

#include <BLIB/Engine/Worlds/World2D.hpp>
#include <Core/World/Cover.hpp>
#include <Core/World/Node.hpp>
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
     * @brief Adds a section of cover (as a rotated rectangle) to the world
     *
     * @param pos The position to add the cover at. Center of the rectangle
     * @param size The side of the cover
     * @param angle The rotation of the cover in degrees
     */
    void addCover(glm::vec2 pos, glm::vec2 size, float angle);

private:
    std::vector<Cover> covers;
    std::vector<Node> nodes;
    // TODO - spatial partitions?
};

} // namespace world
} // namespace core

#endif

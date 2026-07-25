#ifndef CORE_ARENA_GENERATION_SUPERPOSITIONEDNODE_HPP
#define CORE_ARENA_GENERATION_SUPERPOSITIONEDNODE_HPP

#include <Core/Arena/Generation/Domain.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Represents a node being generated with its possible states and constraints
 *
 * @ingroup Arena
 */
struct SuperpositionedNode {
    glm::u32vec2 position;
    float height;
    float moisture;
    Domain domain;

    /**
     * @brief Creates a node with sane defaults
     */
    SuperpositionedNode()
    : position(0, 0)
    , height(0.f)
    , moisture(0.f)
    , domain() {}

    /**
     * @brief Initializes the node
     *
     * @param position The index of the node in the heightmap
     * @param height The height of the node in normalized coordinates
     * @param moisture The normalized moisture of the node
     */
    SuperpositionedNode(const glm::u32vec2& position, float height, float moisture)
    : position(position)
    , height(height)
    , moisture(moisture)
    , domain() {}
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

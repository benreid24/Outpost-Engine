#ifndef CORE_ARENA_NODE_HPP
#define CORE_ARENA_NODE_HPP

#include <Core/Arena/Generation/Biome.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
/**
 * @brief Represents a single node in the arena terrain
 *
 * @ingroup Arena
 */
struct Node {
    glm::u32vec2 index;
    glm::vec2 worldPos;
    float height;
    gen::Biome biome;

    /**
     * @brief Equality operator for track path finding
     *
     * @param other The other node to compare to
     * @return True if the nodes are equal, false otherwise
     */
    bool operator==(const Node& other) const { return worldPos == other.worldPos; }
};

} // namespace arena
} // namespace core

#endif

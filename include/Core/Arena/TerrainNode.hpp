#ifndef CORE_ARENA_TERRAINNODE_HPP
#define CORE_ARENA_TERRAINNODE_HPP

#include <Core/Arena/Generation/Biome.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
/**
 * @brief Represents a single node in the terrain heightmap
 *
 * @ingroup Arena
 */
struct TerrainNode {
    glm::vec2 position;
    float height;
    gen::Biome biome;

    /**
     * @brief Creates the terrain node
     *
     * @param position The position of the node in world coordinates
     * @param height The height of the node in world coordinates
     */
    TerrainNode(const glm::vec2& position, float height)
    : position(position)
    , height(height)
    , biome(gen::Biome::COUNT) {}
};

} // namespace arena
} // namespace core

#endif

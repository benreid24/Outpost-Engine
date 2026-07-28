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
class Seed;

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
    Biome selectedBiome;

    /**
     * @brief Creates a node with sane defaults
     */
    SuperpositionedNode();

    /**
     * @brief Initializes the node
     *
     * @param position The index of the node in the heightmap
     * @param height The height of the node in normalized coordinates
     * @param moisture The normalized moisture of the node
     */
    SuperpositionedNode(const glm::u32vec2& position, float height, float moisture);

    /**
     * @brief Selects from the domain and assigns a biome
     *
     * @param seed The random seed to use
     */
    void collapse(Seed& seed);
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

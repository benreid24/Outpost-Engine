#ifndef CORE_ARENA_GENERATION_SUPERPOSITIONEDNODE_HPP
#define CORE_ARENA_GENERATION_SUPERPOSITIONEDNODE_HPP

#include <BLIB/Containers/PriorityQueue.hpp>
#include <Core/Arena/Generation/AdjacencyBonusProxy.hpp>
#include <Core/Arena/Generation/Domain.hpp>
#include <array>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
namespace gen
{
class Seed;
class Environment;
class ProtoTerrain;

/**
 * @brief Represents a node being generated with its possible states and constraints
 *
 * @ingroup Arena
 */
struct SuperpositionedNode {
    struct Priority {
        std::uint64_t operator()(const SuperpositionedNode* node) const {
            return node->domain.totalWeight();
        }
    };

    glm::u32vec2 position;
    float height;
    float moisture;
    Domain domain;
    Biome selectedBiome;
    std::array<std::vector<AdjacencyBonusProxy>, static_cast<std::size_t>(Biome::COUNT)>
        adjacencyBonuses;
    bl::ctr::PriorityQueueReference<SuperpositionedNode*, Priority> priorityQueueRef;

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
     * @param terrain The terrain being generated
     */
    void collapse(Seed& seed, ProtoTerrain& terrain);

    /**
     * @brief Initializes the proxy objects used to speed up terrain generation
     *
     * @param environment The environment being used to generate the terrain
     */
    void initializeProxies(const Environment& environment);

    /**
     * @brief Updates adjacency bonuses and repositions in the priority queue if necessary
     *
     * @param neighborBiome The newly assigned biome of the neighbor that was collapsed
     */
    void onNeighborCollapsed(Biome neighborBiome);
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

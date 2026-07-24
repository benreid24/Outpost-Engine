#ifndef CORE_ARENA_GENERATION_PARAMETERS_HPP
#define CORE_ARENA_GENERATION_PARAMETERS_HPP

#include <glm/glm.hpp>

/*
 * General approach:
 *   1. Generate a heightmap using Perlin noise
 *   2. Generate a moisture map using Perlin noise
 *   3. Use the heightmap and moisture map to determine the biome domain for each node
 *   4. Perform domain collapse to select a biome for each node
 *     a. Start with the most constrained node
 *     b. Select a biome for the node based on the domain and weights
 *     c. Propagate the constraints to neighboring nodes?
 *     d. Repeat until all nodes have a selected biome
 *   5. Postprocess
 *     a. Resample the heightmap for each node using parameters specific to the biome (can change
 *          octaves to get more/less jagged terrain, etc. Also option to scale height differently)
 *     b. Fill in lakes and rivers via BFS and raise nodes. TODO - how to handle downhill water?
 *   6. Select sites for buildings and other structures
 *     a. Find or create flat areas
 *     b. Flatten sufficiently for the selected structure
 *   7. Place train track via A*
 *     a. Select start and end points for the track
 *     b. Path through from start to end and visit all structures
 *     Note: Can handle bridges and tunnels by adding a large fixed cost based on slop to next node
 *           Cost would normally be distance and slope, but have a max threshold for slope
 *   8. Place trees and rocks based on biome and random noise field
 *      Note: Poisson disk or jittered grid are an option, but can start with high freq Perlin
 *
 */

namespace core
{
namespace arena
{
/// Contains methods and classes for arena generation
namespace gen
{
/**
 * @brief Arena and terrain generation parameters
 *
 * @ingroup Arena
 */
struct Parameters {
    /**
     * @brief Parameters specific to the Perlin noise generator
     */
    struct PerlinParameters {
        float frequency;
        unsigned int octaves;
        float persistence;

        /**
         * @brief Sets sane defaults
         */
        PerlinParameters()
        : frequency(0.01f)
        , octaves(16)
        , persistence(0.5f) {}
    };

    glm::vec2 worldSize;
    float maxHeight;
    float waterHeight;
    PerlinParameters terrainPerlin;
    PerlinParameters moisturePerlin;

    /**
     * @brief Initializes the parameters with sane defaults
     *
     * @param worldSize The size of the world in world coordinates
     * @param maxHeight The maximum height of the terrain in world coordinates
     */
    Parameters(const glm::vec2& worldSize, float maxHeight)
    : worldSize(worldSize)
    , maxHeight(maxHeight)
    , waterHeight(maxHeight * 0.45f)
    , terrainPerlin()
    , moisturePerlin() {}
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

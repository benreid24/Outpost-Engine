#include <Core/Arena/Generation/Generator.hpp>

#include <Core/Arena/Arena.hpp>

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
 *  TODO: Need a way to represent constraints. May want constraints to be additive in addition to
 *        restrictive sometimes
 */

namespace core
{
namespace arena
{
namespace gen
{
Generator::Generator(std::uint64_t seed, const Parameters& parameters)
: seed(seed)
, params(parameters) {
    // TODO - gen heightmap and moisture map
}

void Generator::generate(Arena& output) {
    // TODO
}

} // namespace gen
} // namespace arena
} // namespace core

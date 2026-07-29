#include <Core/Arena/Generation/Generator.hpp>

#include <Core/Arena/Arena.hpp>
#include <Core/Arena/Generation/Environment.hpp>
#include <Core/Arena/Generation/ProtoTerrain.hpp>

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
namespace
{
void samplePerlin(glm::vec2 worldSize, float step, bl::util::Perlin<float>& noise,
                  const Parameters::PerlinParameters& params, bl::ctr::Vector2D<float>& output) {
    const unsigned int xCount = std::ceil(worldSize.x / step) + 0.1f;
    const unsigned int yCount = std::ceil(worldSize.y / step) + 0.1f;

    output.setSize(xCount, yCount, 0.f);
    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf = static_cast<float>(x) * step - worldSize.x * 0.5f;
            const float zf = static_cast<float>(y) * step - worldSize.y * 0.5f;
            float normal   = noise.octave2DNormalized(
                xf * params.frequency, zf * params.frequency, params.octaves, params.persistence);
            normal       = (normal + 1.f) * 0.5f; // map to [0,1]
            output(x, y) = normal;
        }
    }
}
} // namespace

Generator::Generator(std::uint64_t seed, const Parameters& parameters)
: seed(seed)
, params(parameters) {}

void Generator::generate(Arena& output) {
    output.terrain.maxHeight = params.maxHeight;
    output.terrain.worldSize = params.worldSize;

    // generate perlin noise to seed proto terrain
    // TODO - does this produce artifacts?
    bl::util::Perlin<float> heightPerlin   = seed.getPerlin(0);
    bl::util::Perlin<float> moisturePerlin = seed.getPerlin(1);
    samplePerlin(params.worldSize, params.worldStep, heightPerlin, params.terrainPerlin, heightmap);
    samplePerlin(
        params.worldSize, params.worldStep, moisturePerlin, params.moisturePerlin, moistureMap);

    // populate proto terrain
    ProtoTerrain terrain;
    terrain.populate(*this);

    // populate superpositioned node domains
    Environment environment;
    for (const auto& biome : params.biomes) { environment.addRuledBiome(biome); }
    for (unsigned int x = 0; x < terrain.getNodesWidth(); ++x) {
        for (unsigned int y = 0; y < terrain.getNodesHeight(); ++y) {
            environment.domainExpansion(terrain.getNode(x, y));
        }
    }
    terrain.buildPriorityQueue();

    // perform domain collapse to assign biomes
    SuperpositionedNode* mostConstrained = terrain.getMostConstrainedNode();
    while (mostConstrained) {
        mostConstrained->collapse(seed);
        // TODO - we may want to update constraints here
        mostConstrained = terrain.getMostConstrainedNode();
    }

    // postprocess biomes (resample noise, height scale, water, etc)
    // TODO

    // Select and modify locations for train stops
    // TODO

    // route train via modified A*
    // TODO

    // Poisson disk sampling for trees and rocks
    // TODO

    // write to terrain
    output.terrain.heightmap = std::move(heightmap);
    output.terrain.nodes.setSize(heightmap.getWidth(), heightmap.getHeight());
    for (unsigned int x = 0; x < output.terrain.nodes.getWidth(); ++x) {
        for (unsigned int y = 0; y < output.terrain.nodes.getHeight(); ++y) {
            Node& node    = output.terrain.nodes(x, y);
            node.index    = {x, y};
            node.worldPos = glm::vec2(node.index) * params.worldStep;
            node.biome    = terrain.getNode(x, y).selectedBiome;
        }
    }
}

} // namespace gen
} // namespace arena
} // namespace core

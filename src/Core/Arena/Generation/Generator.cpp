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
void samplePerlin(glm::vec2 worldSize, float step, PerlinSampler& sampler,
                  bl::ctr::Vector2D<float>& output) {
    const unsigned int xCount = static_cast<unsigned int>(std::ceil(worldSize.x / step) + 0.1f);
    const unsigned int yCount = static_cast<unsigned int>(std::ceil(worldSize.y / step) + 0.1f);

    output.setSize(xCount, yCount, 0.f);
    for (unsigned int x = 0; x < xCount; ++x) {
        for (unsigned int y = 0; y < yCount; ++y) {
            const float xf = static_cast<float>(x) * step;
            const float zf = static_cast<float>(y) * step;
            output(x, y)   = sampler.sample({xf, zf});
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
    PerlinSampler heightSampler(params.terrainPerlin, params.worldSize * 0.5f, seed, 0);
    PerlinSampler moistureSampler(params.moisturePerlin, params.worldSize * 0.5f, seed, 1);
    samplePerlin(params.worldSize, params.worldStep, heightSampler, heightmap);
    samplePerlin(params.worldSize, params.worldStep, moistureSampler, moistureMap);

    // populate proto terrain
    ProtoTerrain terrain;
    terrain.populate(*this);

    // populate superpositioned node domains
    Environment environment;
    for (const auto& biome : params.biomes) { environment.addRuledBiome(biome); }
    for (unsigned int x = 0; x < terrain.getNodesWidth(); ++x) {
        for (unsigned int y = 0; y < terrain.getNodesHeight(); ++y) {
            SuperpositionedNode& node = terrain.getNode(x, y);
            environment.domainExpansion(node);
            node.initializeProxies(environment);
        }
    }
    terrain.buildPriorityQueue();

    // perform domain collapse to assign biomes
    SuperpositionedNode* mostConstrained = terrain.getMostConstrainedNode();
    while (mostConstrained) {
        mostConstrained->collapse(seed, terrain);
        for (SuperpositionedNode& neighbor :
             terrain.getNodeNeighbors(mostConstrained->position.x, mostConstrained->position.y)) {
            neighbor.onNeighborCollapsed(mostConstrained->selectedBiome);
        }
        mostConstrained = terrain.getMostConstrainedNode();
    }

    // postprocess biomes (resample noise, height scale, water, etc)
    raiseLakes(terrain);
    // TODO - resample noise based on biome
    // TODO - scale heightmap based on biome

    // Select and modify locations for train stops
    // TODO

    // route train via modified A*
    // TODO

    // Poisson disk sampling for trees and rocks
    // TODO

    // write to terrain
    output.terrain.nodes.setSize(heightmap.getWidth(), heightmap.getHeight());
    output.terrain.heightSampler = heightSampler;
    for (unsigned int x = 0; x < output.terrain.nodes.getWidth(); ++x) {
        for (unsigned int y = 0; y < output.terrain.nodes.getHeight(); ++y) {
            Node& node    = output.terrain.nodes(x, y);
            node.index    = {x, y};
            node.worldPos = glm::vec2(node.index) * params.worldStep;
            node.height   = heightmap(x, y);
            node.biome    = terrain.getNode(x, y).selectedBiome;
        }
    }
}

void Generator::raiseLakes(ProtoTerrain& source) {
    bl::ctr::Vector2D<std::uint8_t> visited(heightmap.getWidth(), heightmap.getHeight(), 0);

    for (unsigned int x = 0; x < heightmap.getWidth(); ++x) {
        for (unsigned int y = 0; y < heightmap.getHeight(); ++y) {
            if (visited(x, y) != 0 || source.getNode(x, y).selectedBiome != Biome::Water) {
                continue;
            }

            std::stack<glm::u32vec2> toVisit;
            std::vector<glm::u32vec2> waterNodes;
            float maxHeight = 0.f;

            toVisit.push({x, y});
            visited(x, y) = 1;

            while (!toVisit.empty()) {
                glm::u32vec2 current = toVisit.top();
                toVisit.pop();

                waterNodes.push_back(current);
                maxHeight = std::max(maxHeight, heightmap(current.x, current.y));

                for (const auto& neighbor : source.getNodeNeighbors(current.x, current.y)) {
                    if (visited(neighbor.position.x, neighbor.position.y) == 0 &&
                        neighbor.selectedBiome == Biome::Water) {
                        toVisit.push({neighbor.position.x, neighbor.position.y});
                        visited(neighbor.position.x, neighbor.position.y) = 1;
                    }
                }
            }

            for (const auto& pos : waterNodes) {
                heightmap(pos.x, pos.y)             = maxHeight;
                source.getNode(pos.x, pos.y).height = maxHeight;
                toVisit.push(pos);
            }

            bl::ctr::Vector2D<std::uint8_t> visitedNonWater(
                heightmap.getWidth(), heightmap.getHeight(), 0);
            while (!toVisit.empty()) {
                glm::u32vec2 current = toVisit.top();
                toVisit.pop();

                for (auto& neighbor : source.getNodeNeighbors(current.x, current.y)) {
                    if (visitedNonWater(neighbor.position.x, neighbor.position.y) == 0 &&
                        neighbor.height < maxHeight) {
                        visitedNonWater(neighbor.position.x, neighbor.position.y) = 1;
                        neighbor.selectedBiome                                    = Biome::Water;
                        heightmap(neighbor.position.x, neighbor.position.y)       = maxHeight;
                        neighbor.height                                           = maxHeight;
                        toVisit.push({neighbor.position.x, neighbor.position.y});
                    }
                }
            }
        }
    }
}

} // namespace gen
} // namespace arena
} // namespace core

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

    // Poisson disk sampling for trees and rocks
    // TODO

    // write to terrain
    output.terrain.nodes.setSize(heightmap.getWidth(), heightmap.getHeight());
    output.terrain.heightmap.setSize(heightmap.getWidth() * Terrain::HeightmapRate,
                                     heightmap.getHeight() * Terrain::HeightmapRate);
    for (unsigned int x = 0; x < output.terrain.nodes.getWidth(); ++x) {
        for (unsigned int y = 0; y < output.terrain.nodes.getHeight(); ++y) {
            Node& node    = output.terrain.nodes(x, y);
            node.index    = {x, y};
            node.worldPos = glm::vec2(node.index) * params.worldStep - params.worldSize * 0.5f;
            node.biome    = terrain.getNode(x, y).selectedBiome;

            for (unsigned int ox = 0; ox < Terrain::HeightmapRate; ++ox) {
                for (unsigned int oy = 0; oy < Terrain::HeightmapRate; ++oy) {
                    const unsigned int hx = x * Terrain::HeightmapRate + ox;
                    const unsigned int hy = y * Terrain::HeightmapRate + oy;
                    if (node.biome != Biome::Water) {
                        const float xf =
                            static_cast<float>(hx) * params.worldStep / Terrain::HeightmapRate;
                        const float yf =
                            static_cast<float>(hy) * params.worldStep / Terrain::HeightmapRate;
                        output.terrain.heightmap(hx, hy) = heightSampler.sample({xf, yf});
                    }
                    else { output.terrain.heightmap(hx, hy) = heightmap(x, y); }
                }
            }
        }
    }

    // route train via modified A*
    output.track.generate(output.terrain);

    // select and modify locations for train stop
    identifyTrainStop(output);

    // modify terrain for train stops
    // TODO
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

void Generator::identifyTrainStop(Arena& output) {
    const auto checkSlope = [this, &output](const glm::vec3& start, const glm::vec3& end) -> bool {
        const float slope = std::abs((end.y - start.y) / glm::distance(glm::vec2(start.x, start.z),
                                                                       glm::vec2(end.x, end.z)));
        return slope <= params.stationParams.maxSlope;
    };

    // search the middle 50% of the track for a flat area to create a stop at
    const unsigned int startIndex = output.track.getNodes().size() / 4;
    const unsigned int endIndex   = output.track.getNodes().size() * 3 / 4;

    unsigned int bestStartIndex   = startIndex;
    unsigned int stationNodeCount = 0;
    float bestHeightDiff          = std::numeric_limits<float>::max();
    for (unsigned int i = startIndex; i < endIndex; ++i) {
        float minHeight        = std::numeric_limits<float>::max();
        float maxHeight        = std::numeric_limits<float>::lowest();
        float length           = 0.f;
        unsigned int nodeCount = 0;

        for (unsigned int j = i + 1; j < endIndex && length <= params.stationParams.trackLength;
             ++j) {
            const auto& node     = output.track.getNodes()[j];
            const auto& prevNode = output.track.getNodes()[j - 1];
            if (!checkSlope(prevNode.position, node.position)) { break; }

            minHeight = std::min(minHeight, node.position.y);
            maxHeight = std::max(maxHeight, node.position.y);
            length += node.length;
            ++nodeCount;
        }
        if (nodeCount == 0 || length < params.stationParams.trackLength) { continue; }

        float heightDiff = maxHeight - minHeight;
        if (heightDiff < bestHeightDiff) {
            bestHeightDiff   = heightDiff;
            bestStartIndex   = i;
            stationNodeCount = nodeCount;
        }
    }

    // determine which side of the track to put the station and how big to make it
    bl::ctr::Vector2D<std::uint8_t> visited(
        output.terrain.nodes.getWidth(), output.terrain.nodes.getHeight(), 0);
    std::vector<glm::u32vec2> stationNodes;
    std::stack<glm::u32vec2, std::vector<glm::u32vec2>> toVisit;

    const auto getNodeIndex = [&output](const train::TrackNode& node) {
        return output.getTerrain().worldPosToIndex(glm::vec2(node.position.x, node.position.z));
    };

    const auto validateTrackDistance =
        [this, &output, bestStartIndex, stationNodeCount](const glm::vec3& position) -> bool {
        for (unsigned int i = bestStartIndex; i < bestStartIndex + stationNodeCount; ++i) {
            const auto& node = output.track.getNodes()[i];
            const float dist = glm::distance(glm::vec2(position.x, position.z),
                                             glm::vec2(node.position.x, node.position.z));
            if (dist <= params.stationParams.maxDistanceFromTrack) { return true; }
        }
        return false;
    };

    const auto isTrack = [&output](const glm::u32vec2& index) -> bool {
        for (const auto& node : output.track.getNodes()) {
            const glm::u32vec2 nodeIndex =
                output.getTerrain().worldPosToIndex(glm::vec2(node.position.x, node.position.z));
            if (nodeIndex == index) { return true; }
        }
        return false;
    };

    const auto doBfsStationSide = [this,
                                   &getNodeIndex,
                                   &validateTrackDistance,
                                   &checkSlope,
                                   &isTrack,
                                   &output,
                                   &visited,
                                   &toVisit,
                                   &stationNodes,
                                   bestStartIndex,
                                   stationNodeCount](float side) {
        std::vector<glm::u32vec2> nodeGroup;
        visited.fill(0);

        // add and mark visited all nodes next to the track
        for (unsigned int i = bestStartIndex; i < bestStartIndex + stationNodeCount; ++i) {
            const train::TrackNode& node = output.track.getNodes()[i];
            const glm::u32vec2 nodeIndex = getNodeIndex(node);
            const glm::vec3 right =
                output.track.getRightAtDistance(node.accumulatedDistance) * side;
            const glm::vec2 rightFlat = glm::normalize(glm::vec2(right.x, right.z));
            const glm::u32vec2 offset(rightFlat + glm::vec2(0.5f));

            const glm::u32vec2 stationIndex = nodeIndex + offset;
            nodeGroup.push_back(stationIndex);
            visited(stationIndex.x, stationIndex.y) = 1;
            toVisit.push(stationIndex);
        }

        // search for the station bounds
        while (!toVisit.empty()) {
            const glm::u32vec2 current = toVisit.top();
            toVisit.pop();

            const Node& currentNode = output.terrain.nodes(current.x, current.y);
            const glm::vec3 currentPos(currentNode.worldPos.x,
                                       output.terrain.sampleHeight(currentNode.worldPos),
                                       currentNode.worldPos.y);

            for (int ox = -1; ox <= 1; ++ox) {
                for (int oy = -1; oy <= 1; ++oy) {
                    if (ox == 0 && oy == 0) { continue; }

                    const glm::u32vec2 neighbor(current.x + ox, current.y + oy);
                    if (neighbor.x >= output.terrain.nodes.getWidth() ||
                        neighbor.y >= output.terrain.nodes.getHeight()) {
                        continue;
                    }

                    const Node& neighborNode = output.terrain.nodes(neighbor.x, neighbor.y);
                    if (visited(neighbor.x, neighbor.y) != 0) { continue; }

                    const glm::vec3 neighborPos(neighborNode.worldPos.x,
                                                output.terrain.sampleHeight(neighborNode.worldPos),
                                                neighborNode.worldPos.y);
                    if (!validateTrackDistance(neighborPos)) {
                        visited(neighbor.x, neighbor.y) = 1;
                        continue;
                    }

                    if (!checkSlope(currentPos, neighborPos)) { continue; }

                    visited(neighbor.x, neighbor.y) = 1;
                    nodeGroup.push_back(neighbor);
                    toVisit.push(neighbor);
                }
            }
        }

        if (nodeGroup.size() > stationNodes.size()) { stationNodes = std::move(nodeGroup); }
    };

    // check both sides
    // TODO - this won't actually limit to one side. either we don't care and can do one search, or
    // we need to fix the wall
    doBfsStationSide(1.f);
    doBfsStationSide(-1.f);

    BL_LOG_INFO << "Selected " << stationNodes.size()
                << " nodes for train station with track length of " << stationNodeCount << " nodes";

    // TODO - store this result somehow. need a representation

    float averageHeight = 0.f;
    const float weight  = 1.f / static_cast<float>(stationNodes.size());
    for (const auto& index : stationNodes) {
        const glm::vec2 worldPos = output.terrain.getNodes()(index.x, index.y).worldPos;
        const float height       = output.terrain.sampleHeight(worldPos);
        averageHeight += height * weight;
    }

    // TODO - may want a different terrain modification approach
    for (const auto& index : stationNodes) { output.terrain.modifyHeight(index, averageHeight); }
    output.track.remapToTerrain(output.terrain);
}

} // namespace gen
} // namespace arena
} // namespace core

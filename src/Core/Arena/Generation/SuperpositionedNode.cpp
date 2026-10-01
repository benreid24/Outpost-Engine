#include <Core/Arena/Generation/SuperpositionedNode.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Arena/Generation/Environment.hpp>
#include <Core/Arena/Generation/ProtoTerrain.hpp>
#include <Core/Arena/Generation/Seed.hpp>

namespace core
{
namespace arena
{
namespace gen
{
SuperpositionedNode::SuperpositionedNode()
: position(0, 0)
, height(0.f)
, moisture(0.f)
, domain()
, selectedBiome(Biome::COUNT)
, priorityQueueRef() {}

SuperpositionedNode::SuperpositionedNode(const glm::u32vec2& position, float height, float moisture)
: position(position)
, height(height)
, moisture(moisture)
, domain()
, selectedBiome(Biome::COUNT)
, priorityQueueRef() {}

void SuperpositionedNode::collapse(Seed& seed, ProtoTerrain& terrain) {
    if (domain.biomes.empty()) {
        BL_LOG_WARN << "Node has empty domain. Selecting neighbor node biome";
        bl::ctr::StaticVector<Biome, 8> neighborBiomes;
        for (const auto& neighbor : terrain.getNodeNeighbors(position.x, position.y)) {
            if (neighbor.selectedBiome != Biome::COUNT) {
                neighborBiomes.emplace_back(neighbor.selectedBiome);
            }
        }
        if (!neighborBiomes.empty()) {
            selectedBiome = neighborBiomes[seed.getUint64(0, neighborBiomes.size() - 1)];
        }
        else {
            BL_LOG_WARN << "No valid neighbors to select biome from. Selecting random biome";
            selectedBiome =
                static_cast<Biome>(seed.getUint64(0, static_cast<int>(Biome::COUNT) - 1));
        }
    }
    else { selectedBiome = domain.select(seed); }
}

void SuperpositionedNode::initializeProxies(const Environment& environment) {
    for (WeightedBiome& biome : domain.biomes) {
        const auto& bonuses = environment.getAdjacencyBonuses(biome.biome);
        for (const auto& bonus : bonuses) {
            adjacencyBonuses[static_cast<std::size_t>(biome.biome)].emplace_back(bonus, &biome);
        }
    }
}

void SuperpositionedNode::onNeighborCollapsed(Biome neighborBiome) {
    auto& proxies = adjacencyBonuses[static_cast<std::size_t>(neighborBiome)];
    if (selectedBiome == Biome::COUNT && !proxies.empty()) {
        for (WeightedBiome& biome : domain.biomes) { biome.weight = biome.baseWeight; }
        for (AdjacencyBonusProxy& proxy : proxies) {
            proxy.stackedBonus = proxy.bonus->stack(proxy.stackedBonus, neighborBiome);
            proxy.weightedBiome->weight =
                proxy.bonus->apply(proxy.weightedBiome->weight, proxy.stackedBonus);
        }

        priorityQueueRef.reposition();
    }
}

} // namespace gen
} // namespace arena
} // namespace core

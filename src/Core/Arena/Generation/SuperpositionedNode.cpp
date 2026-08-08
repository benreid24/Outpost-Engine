#include <Core/Arena/Generation/SuperpositionedNode.hpp>

#include <Core/Arena/Generation/Environment.hpp>

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

void SuperpositionedNode::collapse(Seed& seed) { selectedBiome = domain.select(seed); }

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

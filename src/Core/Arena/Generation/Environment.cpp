#include <Core/Arena/Generation/Environment.hpp>

#include <Core/Arena/Generation/SuperpositionedNode.hpp>

namespace core
{
namespace arena
{
namespace gen
{
Environment::Environment() {}

void Environment::addRuledBiome(const RuledBiome& ruledBiome) {
    ruledBiomes.push_back(ruledBiome);
    adjacencyBonusMap[static_cast<std::size_t>(ruledBiome.biome)] = ruledBiome.adjacencyBonuses;
}

void Environment::domainExpansion(SuperpositionedNode& node) const {
    for (const auto& ruledBiome : ruledBiomes) {
        const std::uint64_t heightWeight   = ruledBiome.heightRule.computeWeight(node.height);
        const std::uint64_t moistureWeight = ruledBiome.moistureRule.computeWeight(node.moisture);
        const std::uint64_t totalWeight    = heightWeight * moistureWeight;
        if (totalWeight > 0) { node.domain.biomes.emplace_back(ruledBiome.biome, totalWeight); }
    }
}

} // namespace gen
} // namespace arena
} // namespace core

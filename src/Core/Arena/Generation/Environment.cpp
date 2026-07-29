#include <Core/Arena/Generation/Environment.hpp>

#include <Core/Arena/Generation/SuperpositionedNode.hpp>

namespace core
{
namespace arena
{
namespace gen
{
Environment::Environment() {}

void Environment::addRuledBiome(const RuledBiome& ruledBiome) { ruledBiomes.push_back(ruledBiome); }

void Environment::domainExpansion(SuperpositionedNode& node) const {
    for (const auto& ruledBiome : ruledBiomes) {
        const float heightWeight   = ruledBiome.heightRule.computeWeight(node.height);
        const float moistureWeight = ruledBiome.moistureRule.computeWeight(node.moisture);
        const float totalWeight    = heightWeight * moistureWeight;
        if (totalWeight > 0.f) { node.domain.biomes.emplace_back(ruledBiome.biome, totalWeight); }
    }
}

} // namespace gen
} // namespace arena
} // namespace core

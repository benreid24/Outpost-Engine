#include <Core/Arena/Generation/RuledBiome.hpp>

namespace core
{
namespace arena
{
namespace gen
{
RuledBiome::Rule::Rule()
: allowedRange(0.f, 1.f)
, idealValue(0.5f)
, maxWeight(0.f) {}

RuledBiome::Rule::Rule(float min, float max, float ideal, float weight)
: allowedRange(min, max)
, idealValue(ideal)
, maxWeight(weight) {}

RuledBiome::RuledBiome(Biome biome, const Rule& heightRule, const Rule& moistureRule)
: biome(biome)
, heightRule(heightRule)
, moistureRule(moistureRule) {}

float RuledBiome::Rule::computeWeight(float value) const {
    if (!allowedRange.contains(value)) { return 0.f; }

    if (value >= idealValue) {
        const float range = allowedRange.max - idealValue;
        const float delta = value - idealValue;
        return maxWeight * (1.f - delta / range);
    }
    else {
        const float range = idealValue - allowedRange.min;
        const float delta = idealValue - value;
        return maxWeight * (1.f - delta / range);
    }
}

} // namespace gen
} // namespace arena
} // namespace core

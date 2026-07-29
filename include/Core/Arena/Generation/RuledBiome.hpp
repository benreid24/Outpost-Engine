#ifndef CORE_ARENA_GENERATION_RULEDBIOME_HPP
#define CORE_ARENA_GENERATION_RULEDBIOME_HPP

#include <BLIB/Math/Range.hpp>
#include <Core/Arena/Generation/Biome.hpp>

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Represents a ruleset that determines where a biome can be present in the terrain
 *
 * @ingroup Arena
 */
struct RuledBiome {
    /**
     * @brief Represents a single rule for a biome. Rules govern likilihood based on scalar params
     */
    struct Rule {
        bl::math::Range<float> allowedRange;
        float idealValue;
        float maxWeight;

        /**
         * @brief Creates a rule with full allowed range and no weight
         */
        Rule();

        /**
         * @brief Creates the rule with the given parameters
         *
         * @param min The minimum value of the allowed range
         * @param max The maximum value of the allowed range
         * @param ideal The ideal value for the rule
         * @param weight The maximum weight of the rule when the ideal value is met
         */
        Rule(float min, float max, float ideal, float weight);

        /**
         * @brief Computes the weight of the biome being present based on the input value
         *
         * @param value The terrain node value to compute the weight for
         * @return The probability weight of the biome being present at the node
         */
        float computeWeight(float value) const;
    };

    Biome biome       = Biome::COUNT;
    Rule heightRule   = Rule();
    Rule moistureRule = Rule();

    /**
     * @brief Creates a ruled biome
     *
     * @param biome The biome to represent
     * @param heightRule The rule for the allowed heights of the biome
     * @param moistureRule The rule for the allowed moisture levels of the biome
     */
    RuledBiome(Biome biome, const Rule& heightRule = Rule(), const Rule& moistureRule = Rule());
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

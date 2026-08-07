#ifndef CORE_ARENA_GENERATION_RULEDBIOME_HPP
#define CORE_ARENA_GENERATION_RULEDBIOME_HPP

#include <BLIB/Math/Range.hpp>
#include <Core/Arena/Generation/AdjacencyBonus.hpp>
#include <Core/Arena/Generation/Biome.hpp>
#include <cstdint>
#include <vector>

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
     * @brief Represents a single rule for a biome. Rules govern likelihood based on scalar params
     */
    struct Rule {
        bl::math::Range<float> allowedRange;
        float idealValue;
        std::uint64_t maxWeight;

        /**
         * @brief Computes the weight of the biome being present based on the input value
         *
         * @param value The terrain node value to compute the weight for
         * @return The probability weight of the biome being present at the node
         */
        std::uint64_t computeWeight(float value) const;
    };

    Biome biome                                  = Biome::COUNT;
    Rule heightRule                              = Rule{};
    Rule moistureRule                            = Rule{};
    std::vector<AdjacencyBonus> adjacencyBonuses = std::vector<AdjacencyBonus>{};
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

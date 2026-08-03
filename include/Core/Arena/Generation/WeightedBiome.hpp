#ifndef CORE_ARENA_GENERATION_WEIGHTEDBIOME_HPP
#define CORE_ARENA_GENERATION_WEIGHTEDBIOME_HPP

#include <Core/Arena/Generation/Biome.hpp>

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Represents a possible biome and its weight for a node
 *
 * @ingroup Arena
 */
struct WeightedBiome {
    Biome biome;
    std::uint64_t weight;

    /**
     * @brief Creates the weighted biome
     *
     * @param biome The possible biome
     * @param weight The weight of the biome when selecting a biome for a node
     */
    WeightedBiome(Biome biome, std::uint64_t weight)
    : biome(biome)
    , weight(weight) {}
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

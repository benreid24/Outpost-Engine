#ifndef CORE_ARENA_GENERATION_DOMAIN_HPP
#define CORE_ARENA_GENERATION_DOMAIN_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <Core/Arena/Generation/WeightedBiome.hpp>

namespace core
{
namespace arena
{
namespace gen
{
class Seed;

/**
 * @brief Represents the domain of possible biomes for a node
 *
 * @ingroup Arena
 */
struct Domain {
    bl::ctr::StaticVector<WeightedBiome, static_cast<std::size_t>(Biome::COUNT)> biomes;

    /**
     * @brief Creates the domain with no biomes
     */
    Domain();

    /**
     * @brief Selects a biome from the domain based on the weights of the biomes
     *
     * @param gen The seeded random number generator to use for selection
     * @return The selected biome
     */
    Biome select(Seed& gen) const;

    /**
     * @brief Returns the sum of weights
     */
    float totalWeight() const;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

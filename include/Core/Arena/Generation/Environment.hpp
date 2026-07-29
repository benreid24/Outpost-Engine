#ifndef CORE_ARENA_GENERATION_ENVIRONMENT_HPP
#define CORE_ARENA_GENERATION_ENVIRONMENT_HPP

#include <Core/Arena/Generation/RuledBiome.hpp>
#include <vector>

namespace core
{
namespace arena
{
namespace gen
{
struct SuperpositionedNode;

/**
 * @brief Represents the environment of the arena and its biomes
 *
 * @ingroup Arena
 */
class Environment {
public:
    /**
     * @brief Creates an empty environment
     */
    Environment();

    /**
     * @brief Adds a ruled biome to the environment
     *
     * @param ruledBiome The ruled biome to add
     */
    void addRuledBiome(const RuledBiome& ruledBiome);

    /**
     * @brief Populates the biome domain of the node
     *
     * @param node The node to populate the domain for
     */
    void domainExpansion(SuperpositionedNode& node) const;

    /**
     * @brief Returns the raw list of ruled biomes in the environment
     */
    const std::vector<RuledBiome>& getRuledBiomes() const { return ruledBiomes; }

private:
    std::vector<RuledBiome> ruledBiomes;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

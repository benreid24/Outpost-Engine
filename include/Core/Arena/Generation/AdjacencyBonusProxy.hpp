#ifndef CORE_ARENA_GENERATION_ADJACENCYBONUSPROXY_HPP
#define CORE_ARENA_GENERATION_ADJACENCYBONUSPROXY_HPP

#include <Core/Arena/Generation/AdjacencyBonus.hpp>
#include <cstdint>

namespace core
{
namespace arena
{
namespace gen
{
struct WeightedBiome;

/**
 * @brief Proxy object for quickly updating adjacency bonuses for a node
 *
 * @ingroup Arena
 */
struct AdjacencyBonusProxy {
    const AdjacencyBonus* bonus;
    WeightedBiome* weightedBiome;
    std::uint64_t stackedBonus;

    /**
     * @brief Creates an empty proxy
     */
    AdjacencyBonusProxy(const AdjacencyBonus& bonus, WeightedBiome* weightedBiome)
    : bonus(&bonus)
    , weightedBiome(weightedBiome)
    , stackedBonus(bonus.initialBonus()) {}
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

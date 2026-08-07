#ifndef CORE_ARENA_GENERATION_ADJACENCYBONUS_HPP
#define CORE_ARENA_GENERATION_ADJACENCYBONUS_HPP

#include <Core/Arena/Generation/Biome.hpp>
#include <cstdint>

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Represents a bonus to a biomes weight based on the biome of an adjacent node
 *
 * @ingroup Arena
 */
struct AdjacencyBonus {
    enum struct Relationship { Same, Different };
    enum struct Behavior { Additive, Multiplicative };

    Biome biome;
    Relationship relationship = Relationship::Same;
    Behavior stackBehavior    = Behavior::Multiplicative;
    Behavior applyBehavior    = Behavior::Additive;
    std::uint64_t bonus;

    /**
     * @brief Returns the initial bonus value to use when computing bonuses
     */
    std::uint64_t initialBonus() const {
        switch (stackBehavior) {
        case Behavior::Additive:
            return 0;
        case Behavior::Multiplicative:
            return 1;
        default:
            return 0;
        }
    }

    /**
     * @brief Combines the cumulative bonus with a new bonus based on the behavior
     *
     * @param weight The current weight or bonus to combine with
     * @param newBonus The adjacency bonus to combine with the current weight
     * @param behavior The behavior to use when combining the weights
     * @return The combined weight
     */
    static std::uint64_t combine(std::uint64_t weight, std::uint64_t newBonus, Behavior behavior) {
        switch (behavior) {
        case Behavior::Additive:
            return weight + newBonus;
        case Behavior::Multiplicative:
            return weight * newBonus;
        default:
            return weight;
        }
    }

    /**
     * @brief Applies the bonus from this rule to the base weight for the biome
     *
     * @param baseWeight The base weight for the biome
     * @param bonus The accumulated bonus to apply
     * @return The new biome weight
     */
    std::uint64_t apply(std::uint64_t baseWeight, std::uint64_t bonus) const {
        if (applyBehavior == Behavior::Multiplicative && bonus == 1) { return baseWeight; }
        return combine(baseWeight, bonus, applyBehavior);
    }

    /**
     * @brief Computes the adjacency bonus for the given biome and current bonus
     *
     * @param currentBonus The current cumulative bonus for the biome
     * @param neighbor The biome of the adjacent node to check against
     * @return The new cumulative bonus for the biome after applying the adjacency bonus
     */
    std::uint64_t stack(std::uint64_t currentBonus, Biome neighbor) const {
        switch (relationship) {
        case Relationship::Same:
            if (neighbor == biome) { return combine(currentBonus, bonus, stackBehavior); }
            return currentBonus;
        case Relationship::Different:
            if (neighbor != biome) { return combine(currentBonus, bonus, stackBehavior); }
            return currentBonus;
        default:
            return currentBonus;
        }
    }
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

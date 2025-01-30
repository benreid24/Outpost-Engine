#ifndef CORE_FACTIONS_FILTER_HPP
#define CORE_FACTIONS_FILTER_HPP

#include <Core/Factions/FactionId.hpp>

namespace core
{
namespace fcn
{
/**
 * @brief Filter value to query faction ids with
 *
 * @ingroup Factions
 */
class Filter {
public:
    /**
     * @brief Creates an empty filter
     */
    constexpr Filter()
    : mask(0) {}

    /**
     * @brief Creates a filter to match the given faction id
     *
     * @param id The faction id to match
     */
    Filter(FactionId id)
    : mask(id.id) {}

    /**
     * @brief Creates a filter from a raw bit mask
     *
     * @param mask The bit mask to use to filter
     */
    constexpr Filter(FactionId::IdType mask)
    : mask(mask) {}

    /**
     * @brief Tests whether this filter matches the given faction id
     *
     * @param id The id to test
     * @return True if this filter matches the id, false otherwise
     */
    bool matches(FactionId id) const { return (id.id & mask) != 0; }

    /**
     * @brief Adds an id to this filter
     *
     * @param id The id to add
     */
    void addId(FactionId id) { mask |= id.id; }

    /**
     * @brief Removes an id from this filter
     *
     * @param id The id to remove
     */
    void removeId(FactionId id) { mask &= ~id.id; }

private:
    FactionId::IdType mask;
};

constexpr Filter FilterEmpty = Filter();
constexpr Filter FilterAll   = Filter(FactionId::InternalInvalidId);

} // namespace fcn
} // namespace core

#endif
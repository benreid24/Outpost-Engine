#ifndef CORE_FACTIONS_FACTIONID_HPP
#define CORE_FACTIONS_FACTIONID_HPP

#include <cstdint>
#include <limits>
#include <ostream>

#ifdef BLIB_WINDOWS
#include <intrin.h>
#endif

/**
 * @addtogroup Factions
 * @ingroup Core
 * @brief Combat based factions that units and squads can belong to
 */

namespace core
{
/// Collection of functionality for in game factions
namespace fcn
{
class Filter;

/**
 * @brief Id for factions in the game. Should be treated as an opaque handle
 *
 * @ingroup Factions
 */
class FactionId {
public:
    using IdType                                     = std::uint64_t;
    static constexpr std::size_t MaxFactions         = sizeof(IdType) * 8;
    static constexpr std::uint64_t InternalInvalidId = std::numeric_limits<IdType>::max();

    /**
     * @brief Creates an invalid factions id
     */
    constexpr FactionId()
    : id(InternalInvalidId) {}

    /**
     * @brief Creates a faction from an internal id. Should not be used
     *
     * @param internalId The internal faction id
     */
    constexpr FactionId(IdType internalId)
    : id(internalId) {}

    /// Copies the faction id
    FactionId(const FactionId&) = default;

    /// Copies the faction id
    FactionId(FactionId&&) = default;

    /**
     * @brief Returns a 0-based index for this faction id
     */
    IdType getIndex() const {
#ifdef BLIB_WINDOWS
        if (id == 0) { return 0; }
        unsigned long result;
        _BitScanForward64(&result, id);
        return result;
#else
        return id == 0 ? 0 : __builtin_ctzll(id);
#endif
    }

    /**
     * @brief Returns the underlying faction id value
     */
    IdType getRaw() const { return id; }

    /**
     * @brief Outputs the id
     *
     * @param os Output stream to write to
     * @param id The id to write
     * @return The same output stream
     */
    friend std::ostream& operator<<(std::ostream& os, FactionId id) {
        os << id.id << "(" << id.getIndex() << ")";
        return os;
    }

private:
    IdType id;

    friend class Filter;
};

constexpr FactionId InvalidId = FactionId();

} // namespace fcn
} // namespace core

#endif

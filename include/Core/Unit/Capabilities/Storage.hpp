#ifndef CORE_UNIT_CAPABILITIES_STORAGE_HPP
#define CORE_UNIT_CAPABILITIES_STORAGE_HPP

#include <Core/Unit/Capabilities/Map.hpp>
#include <optional>

namespace core
{
namespace unit
{
namespace able
{
/**
 * @brief Helper struct to store a single capability info struct
 *
 * @tparam C The capability type
 * @ingroup Unit
 */
template<Capability C>
struct SingleStorage {
    std::optional<typename Map<C>::Info> info;
};

/**
 * @brief Helper struct to store info structs for a set of unit capabilities
 *
 * @tparam ...Cs The set of capabilities to possibly store info for
 * @ingroup Unit
 */
template<Capability... Cs>
class Storage : public SingleStorage<Cs>... {
public:
    /// Value representing all possible capabilities
    static constexpr Capability Abilities = (Cs | ...);

    /**
     * @brief Creates empty storage
     */
    Storage() = default;

    /**
     * @brief Adds info for the given capability
     *
     * @tparam C The capability to add info for
     * @return Reference to the new capability info struct
     */
    template<Capability C>
    Map<C>::Info& add() {
        static_assert(C && Abilities, "Capability C must be contained");
        auto& val = SingleStorage<C>::info;
        val.emplace();
        return val.value();
    }

    /**
     * @brief Tests whether the unit has the given capability or not
     *
     * @tparam C The capability to check for
     * @return True if the capability is available, false otherwise
     */
    template<Capability C>
    bool has() {
        if constexpr (Abilities && C) { return SingleStorage<C>::info.has_value(); }
        else { return false; }
    }

    /**
     * @brief Fetches the info struct for the given capability
     *
     * @tparam C The capability to get the info struct for
     * @return The info struct for the given capability or nullptr if not capable
     */
    template<Capability C>
    Map<C>::Info* get() {
        if constexpr (Abilities && C) {
            auto& val = SingleStorage<C>::info;
            return val.has_value() ? &val.value() : nullptr;
        }
        else { return nullptr; }
    }
};

} // namespace able
} // namespace unit
} // namespace core

#endif

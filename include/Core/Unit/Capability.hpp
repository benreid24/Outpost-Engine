#ifndef CORE_UNIT_CAPABILITY_HPP
#define CORE_UNIT_CAPABILITY_HPP

#include <cstdint>
#include <type_traits>

namespace core
{
namespace unit
{
/**
 * @brief Enum representing a unit's capabilities
 *
 * @ingroup Unit
 */
enum struct Capability : std::uint32_t {
    None    = 0,
    Invalid = 0,
    Rotate  = 0x1 << 0,
    Move    = 0x1 << 1,
    Shoot   = 0x1 << 2
};

/**
 * @brief Bitwise OR operator for unit capabilities
 *
 * @param left The left operand
 * @param right The right operand
 * @return The result of the operation
 */
inline constexpr Capability operator|(Capability left, Capability right) {
    using T = std::underlying_type_t<Capability>;
    return static_cast<Capability>(static_cast<T>(left) | static_cast<T>(right));
}

/**
 * @brief Bitwise AND operator for unit capabilities
 *
 * @param left The left operand
 * @param right The right operand
 * @return The result of the operation
 */
inline constexpr Capability operator&(Capability left, Capability right) {
    using T = std::underlying_type_t<Capability>;
    return static_cast<Capability>(static_cast<T>(left) & static_cast<T>(right));
}

/**
 * @brief Bitwise NOT operator for unit capabilities
 *
 * @param c The operand
 * @return The result of the operation
 */
inline constexpr Capability operator~(Capability c) {
    using T = std::underlying_type_t<Capability>;
    return static_cast<Capability>(~static_cast<T>(c));
}

/**
 * @brief Tests whether two capabilities overlap
 *
 * @param left The capability to test for overlap
 * @param right The set of capabilities to overlap with
 * @return True if there is overlap, false if disjoint
 */
inline constexpr bool operator&&(Capability left, Capability right) {
    return (left & right) != Capability::Invalid;
}

} // namespace unit
} // namespace core

#endif

#ifndef CORE_UNIT_CAPABILITIES_MAP_HPP
#define CORE_UNIT_CAPABILITIES_MAP_HPP

#include <Core/Unit/Capabilities/Move.hpp>
#include <Core/Unit/Capabilities/Rotate.hpp>
#include <Core/Unit/Capabilities/Shoot.hpp>
#include <Core/Unit/Capability.hpp>
#include <utility>

namespace core
{
namespace unit
{
namespace able
{
/**
 * @brief Helper to map unit capability to info struct at compile time
 *
 * @tparam Ability The unit ability to get the struct for
 * @ingroup Unit
 */
template<Capability Ability>
struct Map {
    /// The info struct for the ability type
    using Info = std::monostate;
};

template<>
struct Map<Capability::Move> {
    using Info = Move;
};

template<>
struct Map<Capability::Rotate> {
    using Info = Rotate;
};

template<>
struct Map<Capability::Shoot> {
    using Info = Shoot;
};

} // namespace able
} // namespace unit
} // namespace core

#endif

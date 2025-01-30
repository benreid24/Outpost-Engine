#ifndef CORE_UNIT_CAPABILITIES_HPP
#define CORE_UNIT_CAPABILITIES_HPP

#include <Core/Unit/Capabilities/Map.hpp>
#include <Core/Unit/Capabilities/Storage.hpp>
#include <Core/Unit/Capability.hpp>
#include <optional>

namespace core
{
namespace unit
{
/**
 * @brief Storage for a units capability info
 *
 * @ingroup Unit
 */
using Capabilities = able::Storage<Capability::Rotate, Capability::Move, Capability::Shoot>;

} // namespace unit
} // namespace core

#endif

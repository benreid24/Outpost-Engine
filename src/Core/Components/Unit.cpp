#include <Core/Components/Unit.hpp>

namespace core
{
namespace com
{
Unit::Unit(fcn::FactionId faction, bl::com::Physics2D& physics)
: physics(physics)
, faction(faction) {}

} // namespace com
} // namespace core

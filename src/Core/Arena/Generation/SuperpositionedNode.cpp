#include <Core/Arena/Generation/SuperpositionedNode.hpp>

namespace core
{
namespace arena
{
namespace gen
{
SuperpositionedNode::SuperpositionedNode()
: position(0, 0)
, height(0.f)
, moisture(0.f)
, domain() {}

SuperpositionedNode::SuperpositionedNode(const glm::u32vec2& position, float height, float moisture)
: position(position)
, height(height)
, moisture(moisture)
, domain() {}

void SuperpositionedNode::collapse(Seed& seed) { selectedBiome = domain.select(seed); }

} // namespace gen
} // namespace arena
} // namespace core

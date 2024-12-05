#include <Core/Player/HUD/Event.hpp>

#include <Core/Game.hpp>
#include <Core/Player/Player.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
Event::Event(Player& owner, const sf::Event& e)
: windowEvent(e)
, overlayCoord(owner.getRenderObserver().getMousePosInOverlaySpace())
, worldCoord(owner.getRenderObserver().getMousePosInWorldSpace())
, overTarget(owner.getCurrentWorld<world::World>().getTargetAtPosition(worldCoord)) {}

} // namespace hud
} // namespace player
} // namespace core

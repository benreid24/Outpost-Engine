#include <Core/Player/Player.hpp>

namespace core
{
namespace player
{
Player::Player(bl::engine::Engine& engine, bl::rc::Observer* observer, bl::input::Actor* actor)
: bl::engine::Player(engine, observer, actor)
, hud(*this) {}

void Player::onWorldChange(bl::engine::World*, bl::engine::World* world) {
    if (world) { hud.addToOverlay(); }
    else { hud.removeFromOverlay(); }
}

} // namespace player
} // namespace core
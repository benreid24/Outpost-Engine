#include <Core/Player/Player.hpp>

#include <BLIB/Game.hpp>
#include <Core/Game.hpp>

namespace core
{
namespace player
{
Player::Player(bl::engine::Engine& engine, bl::rc::Observer* observer, bl::input::Actor* actor)
: bl::engine::Player(engine, observer, actor)
, hud(*this) {
    // TODO - more ceremonious faction creation?
    auto& game = bl::game::Game::getInstance<Game>();
    faction =
        game.factions().createFaction("Player " + std::to_string(engine.playerCount()))->getId();
}

void Player::onWorldChange(bl::engine::World*, bl::engine::World* world) {
    if (world) { hud.addToOverlay(); }
    else { hud.removeFromOverlay(); }
}

} // namespace player
} // namespace core
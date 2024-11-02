#include <Game/States/CombatDemo.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Game.hpp>
#include <Core/Player/Player.hpp>
#include <Core/World/Collisions.hpp>
#include <Core/World/World.hpp>

namespace game
{
namespace state
{
CombatDemo::CombatDemo(bl::engine::Engine& engine)
: State(bl::engine::StateMask::Running)
, engine(engine) {}

const char* CombatDemo::name() const { return "CombatDemo"; }

void CombatDemo::activate(bl::engine::Engine& engine) {
    auto& game = bl::game::Game::getInstance<core::Game>();
    auto world = engine.getPlayer().enterWorld<core::world::World>();
    world->setLengthUnitScale(60.f / 1920.f);
    engine.renderer().getObserver().setClearColor({0.9f, 0.9f, 1.f, 1.f});
}

void CombatDemo::deactivate(bl::engine::Engine& engine) { engine.getPlayer().leaveWorld(); }

void CombatDemo::update(bl::engine::Engine& engine, float dt, float) {
    engine.getPlayer<core::player::Player>().getHud().update(dt);
}

} // namespace state
} // namespace game

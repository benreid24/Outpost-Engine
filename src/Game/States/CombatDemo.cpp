#include <Game/States/CombatDemo.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <Core/Game.hpp>

namespace game
{
namespace state
{
CombatDemo::CombatDemo()
: State(bl::engine::StateMask::Running) {}

const char* CombatDemo::name() const { return "CombatDemo"; }

void CombatDemo::activate(bl::engine::Engine& engine) {
    auto& game = bl::game::Game::getInstance<core::Game>();
    auto scene = engine.renderer().getObserver().pushScene<bl::rc::scene::Scene2D>();
    engine.renderer().getObserver().setClearColor({0.9f, 0.9f, 1.f, 1.f});

    const auto testEntity = engine.ecs().createEntity(bl::ecs::Flags::WorldObject);
    game.renderSystem().addTestGraphicsToEntity(testEntity, {30.f, 30.f}, sf::Color::Red);
}

void CombatDemo::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
}

void CombatDemo::update(bl::engine::Engine&, float, float) {}

} // namespace state
} // namespace game

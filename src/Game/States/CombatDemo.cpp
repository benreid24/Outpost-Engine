#include <Game/States/CombatDemo.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <Core/Game.hpp>

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
    auto world = engine.getPlayer().enterWorld<bl::engine::World2D>();
    auto scene = world->scene();
    engine.renderer().getObserver().setClearColor({0.9f, 0.9f, 1.f, 1.f});

    bl::event::Dispatcher::subscribe(this);
}

void CombatDemo::deactivate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::unsubscribe(this);
    engine.getPlayer().leaveWorld();
}

void CombatDemo::update(bl::engine::Engine&, float, float) {}

void CombatDemo::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto& game   = bl::game::Game::getInstance<core::Game>();
        auto& ecs    = engine.ecs();
        auto& player = engine.getPlayer();

        const auto newEntity = player.getCurrentWorld().createEntity();

        const auto worldPos = player.getRenderObserver().transformToWorldSpace(
            {event.mouseButton.x, event.mouseButton.y});
        ecs.emplaceComponent<bl::com::Transform2D>(newEntity, worldPos);
        game.renderSystem().addTestGraphicsToEntity(newEntity, {30.f, 30.f}, sf::Color::Blue);
    }
}

} // namespace state
} // namespace game

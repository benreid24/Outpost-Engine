#include <Game/States/CombatDemo.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <Core/Game.hpp>

namespace game
{
namespace state
{
CombatDemo::CombatDemo(bl::engine::Engine& engine)
: State(bl::engine::StateMask::Running)
, engine(engine)
, object(nullptr) {}

const char* CombatDemo::name() const { return "CombatDemo"; }

void CombatDemo::activate(bl::engine::Engine& engine) {
    auto& game = bl::game::Game::getInstance<core::Game>();
    auto world = engine.getPlayer().enterWorld<bl::engine::World2D>();
    world->setLengthUnitScale(60.f / 1920.f);
    engine.renderer().getObserver().setClearColor({0.9f, 0.9f, 1.f, 1.f});

    bl::event::Dispatcher::subscribe(this);
}

void CombatDemo::deactivate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::unsubscribe(this);
    engine.getPlayer().leaveWorld();
}

void CombatDemo::update(bl::engine::Engine&, float dt, float) {
    if (object) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            object->move(core::com::Controllable::Forward);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            object->move(core::com::Controllable::Backward);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            object->move(core::com::Controllable::Left);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            object->move(core::com::Controllable::Right);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            object->rotate(core::com::Controllable::CounterClockwise);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            object->rotate(core::com::Controllable::Clockwise);
        }
    }
}

void CombatDemo::observe(const sf::Event& event) {
    constexpr float Radius = 30.f;

    if (event.type == sf::Event::MouseButtonPressed) {
        // TODO - move spawn methods into helpers/system?
        auto& game   = bl::game::Game::getInstance<core::Game>();
        auto& ecs    = engine.ecs();
        auto& player = engine.getPlayer();

        const auto newEntity = player.getCurrentWorld().createEntity();

        const auto worldPos = player.getRenderObserver().transformToWorldSpace(
            {event.mouseButton.x, event.mouseButton.y});
        auto* transform = ecs.emplaceComponent<bl::com::Transform2D>(newEntity, worldPos);
        game.renderSystem().addTestGraphicsToEntity(newEntity, Radius, sf::Color::Blue);

        auto bodyDef          = b2DefaultBodyDef();
        auto shapeDef         = b2DefaultShapeDef();
        bodyDef.type          = b2_dynamicBody;
        bodyDef.fixedRotation = true;

        ecs.emplaceComponent<bl::com::Hitbox2D>(newEntity, transform, Radius);
        auto physics = game.physicsSystem().addPhysicsToEntity(newEntity, bodyDef, shapeDef);
        object       = ecs.emplaceComponent<core::com::Controllable>(
            newEntity, *physics, 320.f, 1920.f / 6.f, 120.f, 0.9f, 10.f);
    }
}

} // namespace state
} // namespace game

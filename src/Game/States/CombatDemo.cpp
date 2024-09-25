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
    constexpr float Force   = 100.f;
    const float RotateSpeed = 120.f;
    if (object) {
        const float r = bl::math::degreesToRadians(object->getTransform().getRotation());
        const float c = std::cos(r);
        const float s = std::sin(r);
        const glm::vec2 force{c * Force, s * Force};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            object->setLinearDamping(0.f);
            object->applyForceToCenter(force);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            object->setLinearDamping(0.f);
            object->applyForceToCenter(-force);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            object->setLinearDamping(0.f);
            object->applyForceToCenter({force.y, -force.x});
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            object->setLinearDamping(0.f);
            object->applyForceToCenter({-force.y, force.x});
        }
        else { object->setLinearDamping(8.f); }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            object->getTransform().rotate(-RotateSpeed * dt);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            object->getTransform().rotate(RotateSpeed * dt);
        }
    }
}

void CombatDemo::observe(const sf::Event& event) {
    constexpr float Radius = 30.f;

    if (event.type == sf::Event::MouseButtonPressed) {
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
        object = game.physicsSystem().addPhysicsToEntity(newEntity, bodyDef, shapeDef);
    }
}

} // namespace state
} // namespace game

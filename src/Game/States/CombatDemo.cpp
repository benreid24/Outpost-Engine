#include <Game/States/CombatDemo.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Game.hpp>
#include <Core/World/Collisions.hpp>
#include <Core/World/World.hpp>

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
    auto world = engine.getPlayer().enterWorld<core::world::World>();
    world->setLengthUnitScale(60.f / 1920.f);
    engine.renderer().getObserver().setClearColor({0.9f, 0.9f, 1.f, 1.f});

    dragBox.create(*world, {100.f, 100.f});
    dragBox.setFillColor(sf::Color(0, 0, 0, 120));
    dragBox.addToScene(world->scene(), bl::rc::UpdateSpeed::Dynamic);
    dragBox.setHidden(true);

    bl::event::Dispatcher::subscribe(this);
}

void CombatDemo::deactivate(bl::engine::Engine& engine) {
    bl::event::Dispatcher::unsubscribe(this);
    engine.getPlayer().leaveWorld();
}

void CombatDemo::update(bl::engine::Engine&, float dt, float) {
    if (object) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            object->move(core::com::Moveable::Forward);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            object->move(core::com::Moveable::Backward);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            object->move(core::com::Moveable::Left);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            object->move(core::com::Moveable::Right);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            object->rotate(core::com::Moveable::CounterClockwise);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            object->rotate(core::com::Moveable::Clockwise);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { shooter->fire(); }
    }
}

void CombatDemo::observe(const sf::Event& event) {
    constexpr float Radius = 30.f;

    auto& game                = bl::game::Game::getInstance<core::Game>();
    auto& ecs                 = engine.ecs();
    auto& player              = engine.getPlayer();
    core::world::World& world = static_cast<core::world::World&>(player.getCurrentWorld());

    if (event.type == sf::Event::MouseButtonPressed) {
        const auto worldPos = player.getRenderObserver().transformToWorldSpace(
            {event.mouseButton.x, event.mouseButton.y});

        if (event.mouseButton.button == sf::Mouse::Left) {
            // TODO - move spawn methods into helpers/system?

            const auto newEntity = player.getCurrentWorld().createEntity();
            auto* transform      = ecs.emplaceComponent<bl::com::Transform2D>(newEntity, worldPos);
            game.renderSystem().addTestGraphicsToEntity(newEntity, Radius, sf::Color::Blue);

            auto bodyDef          = b2DefaultBodyDef();
            auto shapeDef         = b2DefaultShapeDef();
            bodyDef.type          = b2_dynamicBody;
            bodyDef.fixedRotation = true;
            shapeDef.filter       = core::world::Collisions::getUnitFilter();

            ecs.emplaceComponent<bl::com::Hitbox2D>(newEntity, transform, Radius);
            auto physics = game.physicsSystem().addPhysicsToEntity(newEntity, bodyDef, shapeDef);
            object       = ecs.emplaceComponent<core::com::Moveable>(
                newEntity, *physics, 320.f, 1920.f / 6.f, 270.f, 0.9f, 10.f);

            ecs.emplaceComponent<core::com::Mortal>(newEntity, 100.f);
            shooter = ecs.emplaceComponent<core::com::Shooter>(newEntity, 3.f, 20.f, Radius * 2.f);
        }
        else if (event.mouseButton.button == sf::Mouse::Right) {
            clickStart = worldPos;
            dragBox.setHidden(false);
            dragBox.getTransform().setPosition(clickStart);
        }
        else if (event.mouseButton.button == sf::Mouse::XButton1) {
            world.addNode(core::world::Node::Cover, worldPos);
        }
        else if (event.mouseButton.button == sf::Mouse::XButton2) {
            world.addNode(core::world::Node::Path, worldPos);
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (!dragBox.component().isHidden()) {
            const glm::vec2 eventPos = player.getRenderObserver().transformToWorldSpace(
                {event.mouseMove.x, event.mouseMove.y});
            const glm::vec2 pos  = glm::min(clickStart, eventPos);
            const glm::vec2 size = glm::abs(clickStart - eventPos);
            dragBox.getTransform().setPosition(pos);
            dragBox.scaleToSize(size);
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Right) {
            const glm::vec2 pos = player.getRenderObserver().transformToWorldSpace(
                {event.mouseButton.x, event.mouseButton.y});
            const glm::vec2 center = (clickStart + pos) * 0.5f;
            const glm::vec2 size   = glm::abs(clickStart - pos);
            const float angle      = bl::util::Random::get<float>(0.f, 359.f);
            world.addCover(center, size, angle);
            dragBox.setHidden(true);
        }
    }
}

} // namespace state
} // namespace game

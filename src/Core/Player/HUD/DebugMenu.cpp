#include <Core/Player/HUD/DebugMenu.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Game.hpp>
#include <Core/World/Collisions.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
using namespace bl::gui;

DebugMenu::DebugMenu()
: object(nullptr) {}

void DebugMenu::init(GUI& gui) {
    window = Window::create(LinePacker::create(), "Debug", Window::Default, {0.f, 0.f});

    // TODO - create menu
    auto tabs = Notebook::create();

    auto worldTabs = Notebook::create();
    worldTabs->addPage("nodes", "Nodes", Button::create("Node page here"));
    tabs->addPage("world", "World", worldTabs);

    window->pack(tabs, true, true);
    gui.pack(window, true, true);
    // TODO - show & hide
    // hide();

    auto& game   = bl::game::Game::getInstance<core::Game>();
    auto& engine = game.engine();
    auto& world  = engine.getPlayer().getCurrentWorld();

    dragBox.create(world, {100.f, 100.f});
    dragBox.setFillColor(sf::Color(0, 0, 0, 120));
    dragBox.addToScene(world.scene(), bl::rc::UpdateSpeed::Dynamic);
    dragBox.setHidden(true);
}

void DebugMenu::show() { window->setVisible(true); }

void DebugMenu::hide() { window->setVisible(false); }

bool DebugMenu::processEvent(const sf::Event& event) {
    constexpr float Radius = 30.f;

    auto& game                = bl::game::Game::getInstance<core::Game>();
    auto& engine              = game.engine();
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
        else { return false; }
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
        else { return false; }
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
        else { return false; }
    }
    else { return false; }

    return true;
}

void DebugMenu::update(float dt) {
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

} // namespace hud
} // namespace player
} // namespace core
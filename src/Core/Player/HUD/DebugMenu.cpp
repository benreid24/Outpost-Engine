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
namespace
{
Box::Ptr makeBoxV() { return Box::create(LinePacker::create(LinePacker::Vertical, 4.f)); }
Box::Ptr makeBoxH() { return Box::create(LinePacker::create(LinePacker::Horizontal, 4.f)); }
} // namespace

DebugMenu::DebugMenu()
: object(nullptr) {}

void DebugMenu::init(GUI& gui) {
    window = Window::create(LinePacker::create(), "Debug", Window::Default, {0.f, 0.f});
    window->getCloseButton()
        ->getSignal(Event::LeftClicked)
        .willCall([this](const Event&, Element*) { window->setVisible(false); });

    auto tabs = Notebook::create();
    tabs->addPage(
        "console", "Console", createConsoleTab(), [this]() { currentTopTab = TopTab::Console; });
    tabs->addPage("world", "World", createWorldTab(), [this]() { currentTopTab = TopTab::World; });
    tabs->addPage(
        "entity", "Entity", createEntityTab(), [this]() { currentTopTab = TopTab::Entity; });
    currentTopTab = TopTab::Console;

    window->pack(tabs, true, true);
    gui.pack(window);
    window->setVisible(false);

    auto& game   = bl::game::Game::getInstance<core::Game>();
    auto& engine = game.engine();
    auto& world  = engine.getPlayer().getCurrentWorld();

    dragBox.create(world, {100.f, 100.f});
    dragBox.getTransform().setOrigin(50.f, 50.f);
    dragBox.setFillColor(sf::Color(0, 0, 0, 120));
    dragBox.addToScene(world.scene(), bl::rc::UpdateSpeed::Dynamic);
    dragBox.setHidden(true);
}

Element::Ptr DebugMenu::createConsoleTab() {
    // TODO - create debug console
    return Label::create("Debug console here");
}

Element::Ptr DebugMenu::createWorldTab() {
    auto worldTabs = Notebook::create();

    auto nodeBox    = makeBoxV();
    auto pathNode   = RadioButton::create("Pathing", "path");
    nodeRadioGroup  = pathNode->getRadioGroup();
    auto coverNode  = RadioButton::create("Cover", "cover", nodeRadioGroup);
    auto deleteNode = RadioButton::create("Remove", "remove", nodeRadioGroup);
    pathNode->setValue(true);
    nodeBox->pack(pathNode, true, false);
    nodeBox->pack(coverNode, true, false);
    nodeBox->pack(deleteNode, true, false);
    worldTabs->addPage("nodes", "Nodes", nodeBox, [this]() { currentWorldTab = WorldTab::Nodes; });
    currentWorldTab = WorldTab::Nodes;

    auto coverBox = makeBoxV();
    coverAngle    = Slider::create(Slider::Direction::Horizontal);
    coverAngle->setSliderIncrement(0.0833f);
    coverAngle->setSliderSize(0.1f);
    coverAngle->setRequisition({30.f, 30.f});
    coverAngleLabel = Label::create("Angle: 0");
    coverAngle->getSignal(Event::ValueChanged)
        .willCall(std::bind(&DebugMenu::onCoverAngleChange, this));
    auto placeCover  = RadioButton::create("Place", "place");
    coverRadioGroup  = placeCover->getRadioGroup();
    auto removeCover = RadioButton::create("Remove", "remove", coverRadioGroup);
    placeCover->setValue(true);
    coverBox->pack(coverAngleLabel);
    coverBox->pack(coverAngle, true, false);
    coverBox->pack(placeCover);
    coverBox->pack(removeCover);
    worldTabs->addPage("cover", "Cover", coverBox, [this]() { currentWorldTab = WorldTab::Cover; });

    return worldTabs;
}

Element::Ptr DebugMenu::createEntityTab() {
    auto entityBox = makeBoxH();
    auto leftBox   = makeBoxV();
    auto rightBox  = makeBoxV();
    entityBox->pack(leftBox, true, true);
    entityBox->pack(Separator::create(Separator::Vertical));
    entityBox->pack(rightBox, true, true);

    auto placeEntity   = RadioButton::create("Create", "create");
    entityRadioGroup   = placeEntity->getRadioGroup();
    auto controlEntity = RadioButton::create("Control", "control", entityRadioGroup);
    auto killEntity    = RadioButton::create("Kill", "kill", entityRadioGroup);
    placeEntity->setValue(true);
    leftBox->pack(placeEntity);
    leftBox->pack(controlEntity);
    leftBox->pack(killEntity);

    auto nameRow = makeBoxH();
    nameRow->pack(Label::create("Controlling:"), false, true);
    controlNameLabel = Label::create("<none>");
    controlNameLabel->setColor(sf::Color::Blue, sf::Color::Transparent);
    controlNameLabel->setRequisition({120.f, 20.f});
    nameRow->pack(controlNameLabel, true, true);
    rightBox->pack(nameRow, true, false);

    return entityBox;
}

void DebugMenu::toggle() { window->setVisible(!window->visible()); }

bool DebugMenu::processEvent(const sf::Event& event) {
    if (!window->visible()) { return false; }

    auto& game                = bl::game::Game::getInstance<core::Game>();
    auto& engine              = game.engine();
    auto& ecs                 = engine.ecs();
    auto& player              = engine.getPlayer();
    core::world::World& world = static_cast<core::world::World&>(player.getCurrentWorld());
    const auto worldPos       = player.getRenderObserver().getMousePosInWorldSpace();

    const auto createEntity = [this, &player, &worldPos, &ecs, &game]() {
        constexpr float Radius = 30.f;
        const auto newEntity   = player.getCurrentWorld().createEntity();
        auto* transform        = ecs.emplaceComponent<bl::com::Transform2D>(newEntity, worldPos);
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
    };

    const auto createCover = [this, &event, &worldPos, &world]() -> bool {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                clickStart = worldPos;
                dragBox.setHidden(false);
                dragBox.getTransform().setPosition(clickStart);
                return true;
            }
        }
        else if (!dragBox.component().isHidden()) {
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    const glm::vec2 size = glm::abs(clickStart - worldPos);
                    world.addCover(clickStart, size, getCoverAngle());
                    dragBox.setHidden(true);
                    return true;
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                const glm::vec2 size = glm::abs(clickStart - worldPos);
                dragBox.setSize(size);
                dragBox.getTransform().setOrigin(size * 0.5f);
                return true;
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0.f) { coverAngle->incrementValue(1.f); }
                else { coverAngle->incrementValue(-1.f); }
            }
        }
        return false;
    };

    // process tools
    switch (currentTopTab) {
    case TopTab::World:
        switch (currentWorldTab) {
        case WorldTab::Nodes:
            switch (getCurrentNodeTool()) {
            case NodeTool::CreatePath:
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        world.addNode(core::world::Node::Path, worldPos);
                    }
                }
                break;

            case NodeTool::CreateCover:
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        world.addNode(core::world::Node::Cover, worldPos);
                    }
                }
                break;

            case NodeTool::Remove:
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        return world.removeNodeAtPosition(worldPos);
                    }
                }
            }
            break; // WorldTab::Nodes

        case WorldTab::Cover:
            switch (getCurrentCoverTool()) {
            case CoverTool::Create:
                return createCover();

            case CoverTool::Remove:
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        return world.removeCoverAtPosition(worldPos);
                    }
                }
            }
            break; // WorldTab::Cover
        }
        break; // TopTab::World

    case TopTab::Entity:
        switch (getCurrentEntityTool()) {
        case EntityTool::Create:
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    createEntity();
                    return true;
                }
            }
            break;
        case EntityTool::Control:
            // TODO - query world & set component pointers
            break;
        case EntityTool::Kill:
            // TODO - query world & destroy found entity
            break;
        }
        break; // TopTab::Entity

    case TopTab::Console:
    default:
        return false;
    }

    return false;
}

void DebugMenu::update(float) {
    if (!window->visible()) { return; }

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

DebugMenu::NodeTool DebugMenu::getCurrentNodeTool() const {
    const auto& name = nodeRadioGroup->getActiveButton()->getName();
    if (name == "path") { return NodeTool::CreatePath; }
    if (name == "cover") { return NodeTool::CreateCover; }
    return NodeTool::Remove;
}

DebugMenu::CoverTool DebugMenu::getCurrentCoverTool() const {
    const auto& name = coverRadioGroup->getActiveButton()->getName();
    if (name == "place") { return CoverTool::Create; }
    return CoverTool::Remove;
}

DebugMenu::EntityTool DebugMenu::getCurrentEntityTool() const {
    const auto& name = entityRadioGroup->getActiveButton()->getName();
    if (name == "create") { return EntityTool::Create; }
    if (name == "control") { return EntityTool::Control; }
    return EntityTool::Kill;
}

void DebugMenu::onCoverAngleChange() {
    coverAngleLabel->setText("Angle: " + std::to_string(static_cast<int>(getCoverAngle())));
    dragBox.getTransform().setRotation(getCoverAngle());
}

float DebugMenu::getCoverAngle() const { return std::floor(coverAngle->getValue() * 360.f); }

} // namespace hud
} // namespace player
} // namespace core
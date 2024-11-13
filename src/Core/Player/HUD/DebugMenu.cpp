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

DebugMenu::DebugMenu() {}

void DebugMenu::init(GUI& gui) {
    window = Window::create(LinePacker::create(), "Debug", Window::Default, {0.f, 0.f});
    window->getCloseButton()
        ->getSignal(bl::gui::Event::LeftClicked)
        .willCall([this](const bl::gui::Event&, Element*) { window->setVisible(false); });

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
    coverAngle->getSignal(bl::gui::Event::ValueChanged)
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
    nameRow->pack(Label::create("Hover:"), false, true);
    hoverIdLabel = Label::create("<none>");
    hoverIdLabel->setColor(sf::Color::Blue, sf::Color::Transparent);
    hoverIdLabel->setRequisition({120.f, 20.f});
    nameRow->pack(hoverIdLabel, true, true);
    rightBox->pack(nameRow, true, false);

    nameRow = makeBoxH();
    nameRow->pack(Label::create("Controlling:"), false, true);
    controlNameLabel = Label::create("<none>");
    controlNameLabel->setColor(sf::Color::Blue, sf::Color::Transparent);
    controlNameLabel->setRequisition({120.f, 20.f});
    nameRow->pack(controlNameLabel, true, true);
    rightBox->pack(nameRow, true, false);

    return entityBox;
}

void DebugMenu::toggle() {
    if (window->visible()) {
        window->setVisible(false);
        bl::event::Dispatcher::unsubscribe(this);
    }
    else {
        window->setVisible(true);
        bl::event::Dispatcher::subscribe(this);
    }
}

bool DebugMenu::processEvent(const Event& event) {
    if (!window->visible()) { return false; }

    auto& game                = bl::game::Game::getInstance<core::Game>();
    auto& engine              = game.engine();
    auto& ecs                 = engine.ecs();
    auto& player              = engine.getPlayer();
    core::world::World& world = static_cast<core::world::World&>(player.getCurrentWorld());

    const auto createEntity = [this, &player, &event, &ecs, &game]() {
        constexpr float Radius = 30.f;
        const auto newEntity   = player.getCurrentWorld().createEntity();
        auto* transform =
            ecs.emplaceComponent<bl::com::Transform2D>(newEntity, event.worldPosition());
        game.renderSystem().addTestGraphicsToEntity(newEntity, Radius, sf::Color::Blue);

        auto bodyDef          = b2DefaultBodyDef();
        auto shapeDef         = b2DefaultShapeDef();
        bodyDef.type          = b2_dynamicBody;
        bodyDef.fixedRotation = true;
        shapeDef.filter       = core::world::Collisions::getUnitFilter();

        ecs.emplaceComponent<bl::com::Hitbox2D>(newEntity, transform, Radius);
        auto physics = game.physicsSystem().addPhysicsToEntity(newEntity, bodyDef, shapeDef);
        ecs.emplaceComponent<core::com::Mortal>(newEntity, 100.f);

        controlling.unit = ecs.emplaceComponent<core::com::Unit>(newEntity, *physics);
        controlling.unit->makeMoveable(320.f, 1920.f / 6.f, 270.f, 0.9f);
        controlling.unit->makeShooter(3.f, 20.f, Radius * 2.f);

        onEntityControl(newEntity);
    };

    const auto createCover = [this, &event, &world]() -> bool {
        if (event.source().type == sf::Event::MouseButtonPressed) {
            if (event.source().mouseButton.button == sf::Mouse::Left) {
                clickStart = event.worldPosition();
                dragBox.setHidden(false);
                dragBox.getTransform().setPosition(clickStart);
                return true;
            }
        }
        else if (!dragBox.component().isHidden()) {
            if (event.source().type == sf::Event::MouseButtonReleased) {
                if (event.source().mouseButton.button == sf::Mouse::Left) {
                    const glm::vec2 size = glm::abs(clickStart - event.worldPosition());
                    world.addCover(clickStart, size, getCoverAngle());
                    dragBox.setHidden(true);
                    return true;
                }
            }
            else if (event.source().type == sf::Event::MouseMoved) {
                const glm::vec2 size = glm::abs(clickStart - event.worldPosition());
                dragBox.setSize(size);
                dragBox.getTransform().setOrigin(size * 0.5f);
                return true;
            }
            else if (event.source().type == sf::Event::MouseWheelScrolled) {
                if (event.source().mouseWheelScroll.delta > 0.f) {
                    coverAngle->incrementValue(1.f);
                }
                else { coverAngle->incrementValue(-1.f); }
            }
        }
        return false;
    };

    const auto controlEntity = [this, &event]() -> bool {
        if (event.unit()) {
            controlling.unit = event.unit();
            onEntityControl(event.unit()->getId());
            return true;
        }
        return false;
    };

    const auto killEntity = [&engine, &event]() -> bool {
        if (event.unit()) {
            engine.ecs().destroyEntity(event.unit()->getId());
            return true;
        }
        return false;
    };

    // update hover entity
    if (event.source().type == sf::Event::MouseMoved) {
        const auto* phys = game.physicsSystem().findEntityAtPosition(world, event.worldPosition());
        if (phys) { hoverIdLabel->setText(phys->getOwner().toString()); }
        else { hoverIdLabel->setText("<none>"); }
    }

    // process tools
    switch (currentTopTab) {
    case TopTab::World:
        switch (currentWorldTab) {
        case WorldTab::Nodes:
            switch (getCurrentNodeTool()) {
            case NodeTool::CreatePath:
                if (event.source().type == sf::Event::MouseButtonPressed) {
                    if (event.source().mouseButton.button == sf::Mouse::Left) {
                        world.addNode(core::world::Node::Path, event.worldPosition());
                        return true;
                    }
                }
                break;

            case NodeTool::CreateCover:
                if (event.source().type == sf::Event::MouseButtonPressed) {
                    if (event.source().mouseButton.button == sf::Mouse::Left) {
                        world.addNode(core::world::Node::Cover, event.worldPosition());
                        return true;
                    }
                }
                break;

            case NodeTool::Remove:
                if (event.source().type == sf::Event::MouseButtonPressed) {
                    if (event.source().mouseButton.button == sf::Mouse::Left) {
                        return world.removeNodeAtPosition(event.worldPosition());
                    }
                }
            }
            break; // WorldTab::Nodes

        case WorldTab::Cover:
            switch (getCurrentCoverTool()) {
            case CoverTool::Create:
                return createCover();

            case CoverTool::Remove:
                if (event.source().type == sf::Event::MouseButtonPressed) {
                    if (event.source().mouseButton.button == sf::Mouse::Left) {
                        return world.removeCoverAtPosition(event.worldPosition());
                    }
                }
            }
            break; // WorldTab::Cover
        }
        break; // TopTab::World

    case TopTab::Entity:
        switch (getCurrentEntityTool()) {
        case EntityTool::Create:
            if (event.source().type == sf::Event::MouseButtonPressed) {
                if (event.source().mouseButton.button == sf::Mouse::Left) {
                    createEntity();
                    return true;
                }
            }
            break;
        case EntityTool::Control:
            if (event.source().type == sf::Event::MouseButtonPressed) {
                if (event.source().mouseButton.button == sf::Mouse::Left) {
                    return controlEntity();
                }
            }
        case EntityTool::Kill:
            if (event.source().type == sf::Event::MouseButtonPressed) {
                if (event.source().mouseButton.button == sf::Mouse::Left) { return killEntity(); }
            }
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

    if (controlling.unit) {
        if (controlling.unit->canMove()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                controlling.unit->getMover().move(core::unit::Moveable::Forward);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                controlling.unit->getMover().move(core::unit::Moveable::Backward);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                controlling.unit->getMover().move(core::unit::Moveable::Left);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                controlling.unit->getMover().move(core::unit::Moveable::Right);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                controlling.unit->getMover().rotate(core::unit::Moveable::CounterClockwise);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                controlling.unit->getMover().rotate(core::unit::Moveable::Clockwise);
            }
        }

        if (controlling.unit->canShoot()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                controlling.unit->getShooter().fire();
            }
        }
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

void DebugMenu::onEntityControl(bl::ecs::Entity ent) {
    controlling.entity = ent;
    controlNameLabel->setText(std::to_string(ent.getIndex()));
}

void DebugMenu::observe(const bl::ecs::event::EntityDestroyed& event) {
    if (event.entity == controlling.entity) {
        controlling.reset();
        controlNameLabel->setText("<none>");
    }
}

} // namespace hud
} // namespace player
} // namespace core
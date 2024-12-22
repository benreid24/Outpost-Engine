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
    tabs->addPage("factions", "Factions", createFactionsTab(), [this]() {
        currentTopTab = TopTab::Factions;
        refreshFactions();
    });
    tabs->addPage("entity", "Entity", createEntityTab(), [this]() {
        currentTopTab = TopTab::Entity;
        refreshFactions();
    });
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

Element::Ptr DebugMenu::createFactionsTab() {
    auto content = makeBoxV();

    auto row    = makeBoxH();
    factionName = TextEntry::create();
    factionName->setRequisition({120.f, 30.f});
    auto createBut = Button::create("Create");
    createBut->getSignal(bl::gui::Event::LeftClicked)
        .willCall(std::bind(&DebugMenu::onFactionCreate, this));
    row->pack(Label::create("Name:"), false, true);
    row->pack(factionName, true, true);
    row->pack(createBut, false, true);
    content->pack(row, true, false);

    factionBox = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    factionBox->setMaxSize({600.f, 200.f});
    content->pack(Label::create("Factions:"), true, false);
    content->pack(factionBox, true, false);

    return content;
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
    auto createRow      = makeBoxH();
    entityFactionSelect = ComboBox::create();
    createRow->pack(entityFactionSelect);
    createRow->pack(placeEntity);
    leftBox->pack(createRow);
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
        const int fi = entityFactionSelect->getSelectedOption();
        if (fi < 0 || fi >= factions.size()) { return; }

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
        game.damageSystem().makeMortal(newEntity, factions[fi]->getId(), *physics, 100.f);

        controlling.unit =
            ecs.emplaceComponent<core::com::Unit>(newEntity, factions[fi]->getId(), *physics);
        controlling.unit->capabilities().add<unit::Capability::Move>();
        controlling.unit->capabilities().add<unit::Capability::Rotate>();
        controlling.unit->capabilities().add<unit::Capability::Shoot>();

        ecs.emplaceComponent<core::com::UnitAI>(newEntity, *controlling.unit);

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

    const auto controlEntity = [this, &event, &ecs]() -> bool {
        if (event.target()) {
            auto* unit = ecs.getComponent<com::Unit>(event.target()->getId());
            if (unit) {
                controlling.unit = unit;
                onEntityControl(event.target()->getId());
                return true;
            }
        }
        return false;
    };

    const auto killEntity = [&engine, &event, &ecs]() -> bool {
        if (event.target()) {
            auto* unit = ecs.getComponent<com::Unit>(event.target()->getId());
            if (unit) {
                engine.ecs().destroyEntity(event.target()->getId());
                return true;
            }
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
        auto* mover = controlling.unit->capabilities().get<unit::Capability::Move>();
        if (mover) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                mover->move(unit::able::Move::Forward);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                mover->move(unit::able::Move::Backward);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                mover->move(unit::able::Move::Left);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                mover->move(unit::able::Move::Right);
            }
        }

        auto* rotater = controlling.unit->capabilities().get<unit::Capability::Rotate>();
        if (rotater) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                rotater->rotate(unit::able::Rotate::CounterClockwise);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                rotater->rotate(unit::able::Rotate::Clockwise);
            }
        }

        auto* shooter = controlling.unit->capabilities().get<unit::Capability::Shoot>();
        if (shooter) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { shooter->fire(); }
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

void DebugMenu::onFactionCreate() {
    const std::string& name = factionName->getInput();
    if (name.empty()) { return; }

    auto& game    = bl::game::Game::getInstance<Game>();
    auto* faction = game.factions().createFaction(name.c_str());
    if (faction) {
        factionName->setInput("");
        refreshFactions();
    }
}

void DebugMenu::onFactionDelete(fcn::FactionId id) {
    auto& game = bl::game::Game::getInstance<Game>();
    game.factions().destroyFaction(id);
    refreshFactions();
}

void DebugMenu::refreshFactions() {
    factionBox->clearChildren(true);
    entityFactionSelect->clearOptions();

    auto& game = bl::game::Game::getInstance<Game>();
    game.factions().enumerateFactions(factions);
    for (auto& faction : factions) {
        const std::string name =
            faction->getName() + " (" + std::to_string(faction->getId().getIndex()) + ")";
        entityFactionSelect->addOption(name);

        auto row = makeBoxH();
        auto but = Button::create("Destroy");
        but->getSignal(bl::gui::Event::LeftClicked)
            .willCall(std::bind(&DebugMenu::onFactionDelete, this, faction->getId()));
        row->pack(Label::create(name), true, true);
        row->pack(but, false, true);
        factionBox->pack(row, true, false);
    }
}

} // namespace hud
} // namespace player
} // namespace core
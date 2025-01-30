#include <Core/Player/HUD/TempSquadController.hpp>

#include <BLIB/Game.hpp>
#include <Core/Game.hpp>
#include <Core/Player/Player.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
TempSquadController::TempSquadController(Player& owner)
: owner(owner) {}

void TempSquadController::init() {
    auto& game   = bl::game::Game::getInstance<Game>();
    auto& world  = owner.getCurrentWorld<world::World>();
    auto overlay = owner.getRenderObserver().getOrCreateSceneOverlay();
    auto scene   = world.scene();

    background.create(world, {100.f, 160.f});
    background.setFillColor(sf::Color(160, 160, 160));
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(1.f);
    background.getTransform().setOrigin(100.f, 0.f);
    background.getOverlayScaler().positionInParentSpace({0.88f, 0.f});
    background.getOverlayScaler().scaleToWidthPercent(0.15f);
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);

    controlLabel.create(world, game.defaultFont(), "", 36);
    controlLabel.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    controlLabel.getOverlayScaler().scaleToHeightPercent(0.5f);
    controlLabel.setParent(background);
    controlLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);

    selectRect.create(world, {100.f, 100.f});
    selectRect.setFillColor({0, 20, 245, 100});
    selectRect.component().setContainsTransparency(true);
    selectRect.getTransform().setDepth(100.f);
    selectRect.setHidden(true);
    selectRect.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    controlling = game.squadManager().makeSquad(owner.getFaction());
    makeEmptyState();
}

void TempSquadController::reset() {
    controlLabel.destroy();
    selectRect.destroy();
    background.destroy();
    controlling.release();
}

bool TempSquadController::processEvent(const Event& event) {
    switch (state) {
    case Initial:
        if (event.source().type == sf::Event::MouseButtonPressed &&
            event.source().mouseButton.button == sf::Mouse::Right) {
            dragStart = event.worldPosition();
            state     = Selecting;
            selectRect.setHidden(false);
            return true;
        }
        return false;

    case Selecting:
        switch (event.source().type) {
        case sf::Event::MouseMoved:
            dragEnd = event.worldPosition();
            selectRect.getTransform().setPosition(glm::min(dragStart, dragEnd));
            selectRect.scaleToSize(glm::abs(dragEnd - dragStart));
            break;
        case sf::Event::MouseButtonReleased:
            if (event.source().mouseButton.button == sf::Mouse::Right) {
                const glm::vec2 corner = glm::min(dragStart, dragEnd);
                const glm::vec2 size   = glm::abs(dragStart - dragEnd);
                std::vector<com::Unit*> units =
                    owner.getCurrentWorld<world::World>().getUnitsInArea(
                        {corner.x, corner.y, size.x, size.y});
                controlling->clearUnits();
                for (com::Unit* unit : units) {
                    if (unit->getFaction() == owner.getFaction()) { controlling->addUnit(unit); }
                }
                state = Ordering;
            }
            break;
        }
        return true;

    case Ordering:
        // TODO - handle squad orders + commands
        return false;

    default:
        return false;
    }
}

void TempSquadController::makeEmptyState() {
    controlLabel.getSection().setString("Drag to select");
    controlLabel.getSection().setFillColor(sf::Color::Black);
    centerText();
}

void TempSquadController::makeAttackState() {
    controlLabel.getSection().setString("Attack");
    controlLabel.getSection().setFillColor(sf::Color::Red);
    centerText();
}

void TempSquadController::makeMoveState() {
    controlLabel.getSection().setString("Move");
    controlLabel.getSection().setFillColor(sf::Color::Green);
    centerText();
}

void TempSquadController::centerText() {
    controlLabel.getTransform().setOrigin(controlLabel.getLocalSize() * 0.5f);
}

} // namespace hud
} // namespace player
} // namespace core

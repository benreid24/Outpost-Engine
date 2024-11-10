#include <Core/Player/HUD/TempUnitController.hpp>

#include <Core/Game.hpp>
#include <Core/Player/Player.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
TempUnitController::TempUnitController(Player& owner)
: owner(owner)
, controlling(nullptr) {}

void TempUnitController::init() {
    auto& game  = bl::game::Game::getInstance<Game>();
    auto& world = owner.getCurrentWorld<world::World>();
    auto scene  = owner.getRenderObserver().getOrCreateSceneOverlay();

    background.create(world, {100.f, 60.f});
    background.setFillColor(sf::Color(160, 160, 160));
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(1.f);
    background.getTransform().setOrigin(100.f, 0.f);
    background.getOverlayScaler().positionInParentSpace({1.f, 0.f});
    background.getOverlayScaler().scaleToHeightPercent(0.15f);
    background.addToScene(scene, bl::rc::UpdateSpeed::Static);

    hint.create(world, game.defaultFont(), "", 36); // TODO - font
    hint.getOverlayScaler().positionInParentSpace({0.5f, 0.5f});
    hint.getOverlayScaler().scaleToWidthPercent(0.85f);
    hint.setParent(background);
    hint.addToScene(scene, bl::rc::UpdateSpeed::Static);

    makeEmptyState();
}

void TempUnitController::reset() {
    hint.destroy();
    background.destroy();
}

bool TempUnitController::processEvent(const sf::Event& event) {
    // TODO - make HUD event and perform world query once at event level (add common data like world
    // pos, overlay pos, world unit, etc)
    // TODO - process event
    return false;
}

void TempUnitController::observe(const bl::ecs::event::ComponentRemoved<com::Unit>& event) {
    if (&event.component == controlling) {
        controlling = nullptr;
        // TODO - reset UI state
    }
}

void TempUnitController::makeEmptyState() {
    hint.getSection().setString("Click to command");
    centerText();
}

void TempUnitController::makeAttackState() {
    hint.getSection().setString("Attack unit");
    centerText();
}

void TempUnitController::makeMoveState() {
    hint.getSection().setString("Move here");
    centerText();
}

void TempUnitController::centerText() { hint.getTransform().setOrigin(hint.getLocalSize() * 0.5f); }

} // namespace hud
} // namespace player
} // namespace core

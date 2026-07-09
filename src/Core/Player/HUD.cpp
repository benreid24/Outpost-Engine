#include <Core/Player/HUD.hpp>

#include <Core/Player/Player.hpp>

namespace core
{
namespace player
{
HUD::HUD(Player& owner)
: owner(owner)
, unitController(owner) {}

void HUD::update(float dt) {
    gui->update(dt);
    debugMenu.update(dt);
}

void HUD::addToOverlay() {
    gui = bl::gui::GUI::create(owner.getCurrentWorld(), owner, bl::gui::LinePacker::create());
    debugMenu.init(*gui);
    unitController.init();
    gui->addToOverlay(nullptr, false);
    gui->setRegion({{0.f, 0.f}, {0.f, 0.f}});

    subscribe(bl::engine::Engine::getInstance()->getSignalChannel());
}

void HUD::removeFromOverlay() {
    gui.reset();
    unitController.reset();
    unsubscribe();
}

void HUD::process(const sf::Event& event) {
    if (gui && gui->processEvent(event)) { return; }

    const hud::Event hudEvent(owner, event);
    if (debugMenu.processEvent(hudEvent)) { return; }
    if (unitController.processEvent(hudEvent)) { return; }
}

void HUD::toggleDebugMenu() { debugMenu.toggle(); }

} // namespace player
} // namespace core

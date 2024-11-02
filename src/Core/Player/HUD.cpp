#include <Core/Player/HUD.hpp>

#include <Core/Player/Player.hpp>

namespace core
{
namespace player
{
HUD::HUD(Player& owner)
: owner(owner) {}

void HUD::update(float dt) { gui->update(dt); }

void HUD::addToOverlay() {
    gui = bl::gui::GUI::create(owner.getCurrentWorld(), owner, bl::gui::LinePacker::create());
    debugMenu.init(*gui);
    gui->addToOverlay(nullptr, false);
    gui->setRegion({0.f, 0.f, 0.f, 0.f});
    bl::event::Dispatcher::subscribe(this);
}

void HUD::removeFromOverlay() {
    gui.reset();
    bl::event::Dispatcher::unsubscribe(this);
}

void HUD::observe(const sf::Event& event) {
    if (gui && gui->processEvent(event)) { return; }
    if (debugMenu.processEvent(event)) { return; }
}

} // namespace player
} // namespace core

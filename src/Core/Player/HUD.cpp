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
    gui->addToOverlay();
}

void HUD::removeFromOverlay() { gui.reset(); }

} // namespace player
} // namespace core

#include <Core/Player/HUD/DebugMenu.hpp>

namespace core
{
namespace player
{
namespace hud
{
using namespace bl::gui;

void DebugMenu::init(GUI& gui) {
    window = Window::create(LinePacker::create(), "Debug", Window::Default, {0.f, 0.f});

    // TODO - create menu

    gui.pack(window);
}

void DebugMenu::show() { window->setVisible(true); }

void DebugMenu::hide() { window->setVisible(false); }

} // namespace hud
} // namespace player
} // namespace core
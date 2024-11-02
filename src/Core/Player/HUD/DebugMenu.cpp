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
    auto tabs = Notebook::create();

    auto worldTabs = Notebook::create();
    worldTabs->addPage("nodes", "Nodes", Button::create("Node page here"));
    tabs->addPage("world", "World", worldTabs);

    window->pack(tabs, true, true);
    gui.pack(window, true, true);
    // hide();
}

void DebugMenu::show() { window->setVisible(true); }

void DebugMenu::hide() { window->setVisible(false); }

} // namespace hud
} // namespace player
} // namespace core
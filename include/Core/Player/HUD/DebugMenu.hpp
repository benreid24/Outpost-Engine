#ifndef CORE_PLAYER_HUD_DEBUGMENU_HPP
#define CORE_PLAYER_HUD_DEBUGMENU_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace core
{
namespace player
{
/// Collection of menu classes for the player HUD
namespace hud
{
/**
 * @brief Debug menu class for the player HUD
 *
 * @ingroup Player
 */
class DebugMenu {
public:
    /**
     * @brief Creates the GUI components in the hidden state
     *
     * @param gui The HUD GUI object
     */
    void init(bl::gui::GUI& gui);

    /**
     * @brief Shows the debug menu window
     */
    void show();

    /**
     * @brief Hides the debug menu window
     */
    void hide();

private:
    bl::gui::Window::Ptr window;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

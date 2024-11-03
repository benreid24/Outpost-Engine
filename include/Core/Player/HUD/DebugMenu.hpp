#ifndef CORE_PLAYER_HUD_DEBUGMENU_HPP
#define CORE_PLAYER_HUD_DEBUGMENU_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Components/Moveable.hpp>
#include <Core/Components/Shooter.hpp>

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
     * @brief Initializes some default state
     */
    DebugMenu();

    /**
     * @brief Updates the debug menu
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Creates the GUI components in the hidden state
     *
     * @param gui The HUD GUI object
     */
    void init(bl::gui::GUI& gui);

    /**
     * @brief Shows or hides the debug menu window
     */
    void toggle();

    /**
     * @brief Processes the window event
     *
     * @param event The event to process
     * @return True if the event was used, false if no effect
     */
    bool processEvent(const sf::Event& event);

private:
    bl::gui::Window::Ptr window;

    core::com::Moveable* object;
    core::com::Shooter* shooter;
    bl::gfx::Rectangle dragBox;
    glm::vec2 clickStart;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

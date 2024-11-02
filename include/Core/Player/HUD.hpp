#ifndef CORE_PLAYER_HUD_HPP
#define CORE_PLAYER_HUD_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Player/HUD/DebugMenu.hpp>

namespace core
{
namespace player
{
class Player;

/**
 * @brief Top level HUD class for the player
 *
 * @ingroup Player
 */
class HUD {
public:
    /**
     * @brief Creates the HUD
     *
     * @param owner The player that the HUD belongs to
     */
    HUD(Player& owner);

    /**
     * @brief Updates the HUD
     *
     * @param dt Elapsed time in seconds
     */
    void update(float dt);

    /**
     * @brief Adds the HUD to the players current overlay
     */
    void addToOverlay();

    /**
     * @brief Removes the HUD from the overlay that it is on
     */
    void removeFromOverlay();

private:
    Player& owner;
    bl::gui::GUI::Ptr gui;
    hud::DebugMenu debugMenu;
};

} // namespace player
} // namespace core

#endif

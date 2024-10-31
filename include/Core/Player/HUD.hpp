#ifndef CORE_PLAYER_HUD_HPP
#define CORE_PLAYER_HUD_HPP

#include <BLIB/Interfaces/GUI.hpp>

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
};

} // namespace player
} // namespace core

#endif

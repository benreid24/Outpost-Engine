#ifndef CORE_PLAYER_HUD_EVENT_HPP
#define CORE_PLAYER_HUD_EVENT_HPP

#include <Core/Components/Unit.hpp>
#include <SFML/Window/Event.hpp>

namespace core
{
namespace player
{
class Player;

namespace hud
{
/**
 * @brief HUD event. Wrapper around sf::Event with some common fields computed
 *
 * @ingroup HUD
 */
class Event {
public:
    /**
     * @brief Creates the event
     *
     * @param owner The player processing the event
     * @param event The source window event
     */
    Event(Player& owner, const sf::Event& event);

    /**
     * @brief Returns the source window event
     */
    const sf::Event& source() const { return windowEvent; }

    /**
     * @brief Returns the unit the event occurred over. May be nullptr
     */
    com::Unit* unit() const { return overUnit; }

    /**
     * @brief Returns the position of the event in overlay coordinates
     */
    const glm::vec2& overlayPosition() const { return overlayCoord; }

    /**
     * @brief Returns the position of the event in world coordinates
     */
    const glm::vec2& worldPosition() const { return worldCoord; }

private:
    const sf::Event& windowEvent;
    glm::vec2 overlayCoord;
    glm::vec2 worldCoord;
    com::Unit* overUnit;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

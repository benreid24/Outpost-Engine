#ifndef CORE_PLAYER_HUD_TEMPSQUADCONTROLLER_HPP
#define CORE_PLAYER_HUD_TEMPSQUADCONTROLLER_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <Core/Player/HUD/Event.hpp>
#include <Core/Unit/Squad.hpp>

namespace core
{
namespace player
{
class Player;

namespace hud
{
class TempSquadController {
public:
    TempSquadController(Player& player);

    void init();

    void reset();

    bool processEvent(const Event& event);

private:
    enum State { Initial, Selecting, Ordering };

    Player& owner;
    unit::Squad* controlling;
    State state;
    glm::vec2 dragStart;
    glm::vec2 dragEnd;

    bl::gfx::Rectangle background;
    bl::gfx::Text controlLabel;
    bl::gfx::Rectangle selectRect;

    void makeEmptyState();
    void makeMoveState();
    void makeAttackState();
    void centerText();
};

} // namespace hud
} // namespace player
} // namespace core

#endif

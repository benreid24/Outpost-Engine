#ifndef CORE_PLAYER_HUD_TEMPSQUADCONTROLLER_HPP
#define CORE_PLAYER_HUD_TEMPSQUADCONTROLLER_HPP

#include <BLIB/Objects/Rectangle.hpp>
#include <BLIB/Objects/Text.hpp>
#include <Core/Player/HUD/Event.hpp>
#include <Core/Unit/Squad.hpp>
#include <Core/Unit/SquadManager.hpp>

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
    unit::SquadManager::Ref controlling;
    State state;
    glm::vec2 dragStart;
    glm::vec2 dragEnd;

    bl::obj::Rectangle background;
    bl::obj::Text controlLabel;
    bl::obj::Rectangle selectRect;

    void makeEmptyState();
    void makeMoveState();
    void makeAttackState();
    void centerText();
};

} // namespace hud
} // namespace player
} // namespace core

#endif

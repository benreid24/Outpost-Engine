#ifndef CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP
#define CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP

#include <BLIB/ECS/Events.hpp>
#include <BLIB/Objects/Rectangle.hpp>
#include <BLIB/Objects/Text.hpp>
#include <BLIB/Signals.hpp>
#include <Core/Components/UnitAI.hpp>
#include <Core/Player/HUD/Event.hpp>

namespace core
{
namespace player
{
class Player;

namespace hud
{
class TempUnitController : public bl::sig::Listener<bl::ecs::event::ComponentRemoved<com::UnitAI>> {
public:
    TempUnitController(Player& owner);

    void init();

    void reset();

    bool processEvent(const Event& event);

private:
    Player& owner;
    bl::obj::Rectangle background;
    bl::obj::Text hint;
    com::UnitAI* controlling;
    com::Combatant* self;

    void makeEmptyState();
    void makeMoveState();
    void makeAttackState();
    void centerText();
    virtual void process(const bl::ecs::event::ComponentRemoved<com::UnitAI>& event) override;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

#ifndef CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP
#define CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP

#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <Core/Components/UnitAI.hpp>
#include <Core/Player/HUD/Event.hpp>

namespace core
{
namespace player
{
class Player;

namespace hud
{
class TempUnitController
: public bl::event::Listener<bl::ecs::event::ComponentRemoved<com::UnitAI>> {
public:
    TempUnitController(Player& owner);

    void init();

    void reset();

    bool processEvent(const Event& event);

private:
    Player& owner;
    bl::gfx::Rectangle background;
    bl::gfx::Text hint;
    com::UnitAI* controlling;
    com::Combatant* self;

    void makeEmptyState();
    void makeMoveState();
    void makeAttackState();
    void centerText();
    virtual void observe(const bl::ecs::event::ComponentRemoved<com::UnitAI>& event) override;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

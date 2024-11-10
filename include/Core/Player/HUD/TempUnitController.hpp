#ifndef CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP
#define CORE_PLAYER_HUD_TEMPUNITCONTROLLER_HPP

#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <Core/Components/Unit.hpp>

namespace core
{
namespace player
{
class Player;

namespace hud
{
class TempUnitController : public bl::event::Listener<bl::ecs::event::ComponentRemoved<com::Unit>> {
public:
    TempUnitController(Player& owner);

    void init();

    void reset();

    bool processEvent(const sf::Event& event);

private:
    Player& owner;
    bl::gfx::Rectangle background;
    bl::gfx::Text hint;
    com::Unit* controlling;

    void makeEmptyState();
    void makeMoveState();
    void makeAttackState();
    void centerText();
    virtual void observe(const bl::ecs::event::ComponentRemoved<com::Unit>& event) override;
};

} // namespace hud
} // namespace player
} // namespace core

#endif

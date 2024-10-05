#ifndef GAME_STATES_COMBATDEMO_HPP
#define GAME_STATES_COMBATDEMO_HPP

#include <BLIB/Engine/State.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <Core/Components/Moveable.hpp>
#include <Core/Components/Shooter.hpp>
#include <SFML/Window/Event.hpp>

namespace game
{
namespace state
{
class CombatDemo
: public bl::engine::State
, public bl::event::Listener<sf::Event> {
public:
    CombatDemo(bl::engine::Engine& engine);

    virtual ~CombatDemo() = default;

private:
    bl::engine::Engine& engine;
    core::com::Moveable* object;
    core::com::Shooter* shooter;

    bl::gfx::Rectangle dragBox;
    glm::vec2 clickStart;

    const char* name() const override;
    void activate(bl::engine::Engine& engine) override;
    void deactivate(bl::engine::Engine& engine) override;
    void update(bl::engine::Engine& engine, float dt, float realDt) override;

    virtual void observe(const sf::Event& event) override;
};

} // namespace state
} // namespace game

#endif

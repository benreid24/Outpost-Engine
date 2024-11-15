#ifndef GAME_STATES_COMBATDEMO_HPP
#define GAME_STATES_COMBATDEMO_HPP

#include <BLIB/Engine/State.hpp>
#include <BLIB/Events.hpp>
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

    const char* name() const override;
    void activate(bl::engine::Engine& engine) override;
    void deactivate(bl::engine::Engine& engine) override;
    void update(bl::engine::Engine& engine, float dt, float realDt) override;
    virtual void observe(const sf::Event& event) override;
};

} // namespace state
} // namespace game

#endif

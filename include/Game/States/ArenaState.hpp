#ifndef GAME_STATES_ARENASTATE_HPP
#define GAME_STATES_ARENASTATE_HPP

#include <BLIB/Engine/State.hpp>
#include <BLIB/Graphics/Terrain.hpp>
#include <BLIB/Signals.hpp>
#include <Core/Arena/Arena.hpp>
#include <SFML/Window/Event.hpp>

namespace game
{
namespace state
{
/**
 * @brief Game state for the arena simulation
 *
 * @ingroup States
 */
class ArenaState
: public bl::engine::State
, public bl::sig::Listener<sf::Event> {
public:
    /**
     * @brief Creates the state
     */
    ArenaState();

    /**
     * @brief Destroys the state
     */
    virtual ~ArenaState() = default;

private:
    core::arena::Arena arena;

    const char* name() const override;
    void activate(bl::engine::Engine& engine) override;
    void deactivate(bl::engine::Engine& engine) override;
    void update(bl::engine::Engine& engine, float dt, float realDt) override;
    virtual void process(const sf::Event& event) override;
};

} // namespace state
} // namespace game

#endif

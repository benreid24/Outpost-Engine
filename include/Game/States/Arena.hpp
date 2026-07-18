#ifndef GAME_STATES_ARENA_HPP
#define GAME_STATES_ARENA_HPP

#include <BLIB/Engine/State.hpp>
#include <BLIB/Graphics/Terrain.hpp>
#include <BLIB/Signals.hpp>
#include <SFML/Window/Event.hpp>

namespace game
{
namespace state
{
class Arena
: public bl::engine::State
, public bl::sig::Listener<sf::Event> {
public:
    Arena();

    virtual ~Arena() = default;

private:
    bl::util::Perlin<float> perlin;
    bl::gfx::Terrain terrain;

    void colorTerrain();
    const char* name() const override;
    void activate(bl::engine::Engine& engine) override;
    void deactivate(bl::engine::Engine& engine) override;
    void update(bl::engine::Engine& engine, float dt, float realDt) override;
    virtual void process(const sf::Event& event) override;
};

} // namespace state
} // namespace game

#endif

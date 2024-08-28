#ifndef GAME_STATES_COMBATDEMO_HPP
#define GAME_STATES_COMBATDEMO_HPP

#include <BLIB/Engine/State.hpp>

namespace game
{
namespace state
{
class CombatDemo : public bl::engine::State {
public:
    CombatDemo();

    virtual ~CombatDemo() = default;

private:
    const char* name() const override;
    void activate(bl::engine::Engine& engine) override;
    void deactivate(bl::engine::Engine& engine) override;
    void update(bl::engine::Engine& engine, float dt, float realDt) override;
};

} // namespace state
} // namespace game

#endif

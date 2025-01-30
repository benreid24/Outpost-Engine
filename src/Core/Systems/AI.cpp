#include <Core/Systems/AI.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <Core/Game.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace sys
{
void AI::init(bl::engine::Engine& e) {
    engine  = &e;
    unitAIs = &e.ecs().getAllComponents<com::UnitAI>();
}

void AI::update(std::mutex&, float dt, float, float, float) {
    auto& game = bl::game::Game::getInstance<Game>();
    unitAIs->forEach([this, dt, &game](bl::ecs::Entity entity, com::UnitAI& ai) {
        auto world = engine->getWorld<world::World>(entity.getWorldIndex());
        const unit::ai::UpdateContext ctx{game, *world, dt};
        ai.update(ctx);
    });
}

} // namespace sys
} // namespace core

#include <Core/Systems/EntityActions.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace core
{
namespace sys
{
EntityActions::EntityActions()
: pool(nullptr) {}

void EntityActions::init(bl::engine::Engine& engine) {
    pool = &engine.ecs().getAllComponents<com::Controllable>();
}

void EntityActions::update(std::mutex&, float dt, float, float, float) {
    pool->forEach([dt](bl::ecs::Entity, com::Controllable& m) { m.apply(dt); });
}

} // namespace sys
} // namespace core

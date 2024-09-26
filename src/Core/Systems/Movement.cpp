#include <Core/Systems/Movement.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace core
{
namespace sys
{
Movement::Movement()
: pool(nullptr) {}

void Movement::init(bl::engine::Engine& engine) {
    pool = &engine.ecs().getAllComponents<com::Moveable>();
}

void Movement::update(std::mutex&, float dt, float, float, float) {
    pool->forEach([dt](bl::ecs::Entity, com::Moveable& m) { m.apply(dt); });
}

} // namespace sys
} // namespace core

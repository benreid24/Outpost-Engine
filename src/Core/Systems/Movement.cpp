#include <Core/Systems/Movement.hpp>

#include <Core/Components/WorldNode.hpp>

namespace core
{
namespace sys
{
Movement::~Movement() { bl::event::Dispatcher::unsubscribe(this); }

void Movement::init(bl::engine::Engine& e) {
    engine = &e;
    bl::event::Dispatcher::subscribe(this);
}

void Movement::update(std::mutex&, float dt, float, float, float) {}

void Movement::observe(const bl::sys::Physics2D::SensorEntered& event) {
    com::WorldNode* node = engine->ecs().getComponent<com::WorldNode>(event.sensor);
    if (node) { node->world->handleSensorEnter(node->node, event.entity); }
}

void Movement::observe(const bl::sys::Physics2D::SensorExited& event) {
    com::WorldNode* node = engine->ecs().getComponent<com::WorldNode>(event.sensor);
    if (node) { node->world->handleSensorExit(node->node, event.entity); }
}

} // namespace sys
} // namespace core

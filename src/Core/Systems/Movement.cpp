#include <Core/Systems/Movement.hpp>

#include <Core/Components/WorldNode.hpp>

namespace core
{
namespace sys
{
Movement::~Movement() {}

void Movement::init(bl::engine::Engine& e) {
    engine = &e;
    subscribe(e.getSignalChannel());
}

void Movement::update(std::mutex&, float dt, float, float, float) {}

void Movement::process(const bl::sys::Physics2D::SensorEntered& event) {
    com::WorldNode* node = engine->ecs().getComponent<com::WorldNode>(event.sensor);
    if (node) { node->world->handleSensorEnter(node->node, event.entity); }
}

void Movement::process(const bl::sys::Physics2D::SensorExited& event) {
    com::WorldNode* node = engine->ecs().getComponent<com::WorldNode>(event.sensor);
    if (node) { node->world->handleSensorExit(node->node, event.entity); }
}

} // namespace sys
} // namespace core

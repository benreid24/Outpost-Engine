#include <Core/Systems/Render.hpp>

#include <BLIB/Graphics/Circle.hpp>
#include <BLIB/Graphics/Rectangle.hpp>

namespace core
{
namespace sys
{
Render::Render()
: engine(nullptr) {}

void Render::addTestGraphicsToEntity(bl::ecs::Entity entity, glm::vec2 size, bl::rc::Color color) {
    bl::gfx::Rectangle box;
    box.deleteEntityOnDestroy(false);
    box.create(*engine->getWorld(entity.getWorldIndex()), size);
    box.setFillColor(color);
    box.getTransform().setOrigin(size * 0.5f);
    box.commit();

    box.addToScene(engine->renderer().getObserver().getCurrentScene(),
                   bl::rc::UpdateSpeed::Dynamic);
}

void Render::addTestGraphicsToEntity(bl::ecs::Entity entity, float radius, bl::rc::Color color) {
    bl::gfx::Circle circle;
    circle.deleteEntityOnDestroy(false);
    circle.create(*engine->getWorld(entity.getWorldIndex()), entity, radius);
    circle.setFillColor(color);
    circle.getTransform().setOrigin(radius, radius);
    circle.commit();

    auto& ib = circle.component().indexBuffer;
    for (unsigned int i = 0; i < 12; ++i) {
        const unsigned int j = (295 + i) % 300;
        if (j != 0) { ib.vertices()[j].color = {0.f, 0.f, 0.f, 1.f}; }
    }

    circle.addToScene(engine->renderer().getObserver().getCurrentScene(),
                      bl::rc::UpdateSpeed::Dynamic);
}

void Render::init(bl::engine::Engine& e) { engine = &e; }

void Render::update(std::mutex&, float, float, float, float) {}

} // namespace sys
} // namespace core

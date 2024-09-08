#include <Core/Systems/Render.hpp>

#include <BLIB/Graphics/Circle.hpp>

namespace core
{
namespace sys
{
Render::Render()
: engine(nullptr) {}

void Render::addTestGraphicsToEntity(bl::ecs::Entity entity, glm::vec2 size, sf::Color color) {
    constexpr float Radius = 100.f;
    constexpr glm::vec4 Black{0.f, 0.f, 0.f, 1.f};

    bl::gfx::Circle circle;
    circle.deleteEntityOnDestroy(false);
    circle.create(*engine->getWorld(entity.getWorldIndex()), entity, Radius);
    circle.setFillColor(color);
    circle.scaleToSize(size);
    circle.getTransform().setOrigin(Radius, Radius);
    circle.commit();

    auto& ib = circle.component().indexBuffer;
    for (unsigned int i = 0; i < 10; ++i) {
        const unsigned int j = (295 + i) % 300;
        if (j != 0) { ib.vertices()[j].color = Black; }
    }

    circle.addToScene(engine->renderer().getObserver().getCurrentScene(),
                      bl::rc::UpdateSpeed::Dynamic);
}

void Render::init(bl::engine::Engine& e) { engine = &e; }

void Render::update(std::mutex&, float, float, float, float) {}

} // namespace sys
} // namespace core

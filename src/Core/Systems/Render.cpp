#include <Core/Systems/Render.hpp>

#include <BLIB/Graphics/Circle.hpp>

namespace core
{
namespace sys
{
Render::Render()
: engine(nullptr) {}

void Render::addTestGraphicsToEntity(bl::ecs::Entity entity, glm::vec2 size, sf::Color color) {
    constexpr glm::vec4 Black{0.f, 0.f, 0.f, 1.f};
    const float radius = glm::length(size);

    bl::gfx::Circle circle;
    circle.deleteEntityOnDestroy(false);
    circle.create(*engine, entity, radius);
    circle.setFillColor(color);
    circle.commit();

    auto& ib                 = circle.component().indexBuffer;
    ib.vertices()[1].color   = Black;
    ib.vertices()[2].color   = Black;
    ib.vertices()[299].color = Black;
    ib.vertices()[300].color = Black;
}

void Render::init(bl::engine::Engine& e) { engine = &e; }

void Render::update(std::mutex&, float, float, float, float) {}

} // namespace sys
} // namespace core

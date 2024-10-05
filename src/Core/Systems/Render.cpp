#include <Core/Systems/Render.hpp>

#include <BLIB/Graphics/Circle.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/VertexBuffer2D.hpp>

namespace core
{
namespace sys
{
Render::Render()
: engine(nullptr) {}

void Render::addTestGraphicsToEntity(bl::ecs::Entity entity, glm::vec2 size, bl::rc::Color color) {
    bl::gfx::Rectangle box;
    box.deleteEntityOnDestroy(false);
    box.create(*engine->getWorld(entity.getWorldIndex()), entity, size);
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

void Render::addDebugGraphicsToNode(const world::Node& node) {
    constexpr unsigned int CircleTriangleCount = 100;
    constexpr unsigned int CircleVertexCount   = CircleTriangleCount * 3;
    constexpr unsigned int ArrowCount          = 12;
    constexpr unsigned int ArrowTriangleCount  = 1;
    constexpr unsigned int ArrowVertexCount    = ArrowCount * ArrowTriangleCount * 3;
    constexpr unsigned int VertexCount         = CircleVertexCount + ArrowVertexCount;

    constexpr float Radius     = 10.f;
    constexpr float ArrowWidth = 6.f;

    bl::gfx::VertexBuffer2D vb;
    vb.create(engine->getPlayer().getCurrentWorld(), VertexCount);
    vb.deleteEntityOnDestroy(false);
    vb.getTransform().setOrigin(Radius * 0.5f, Radius * 0.5f);
    vb.getTransform().setPosition(node.getPosition());
    vb.getTransform().setDepth(1.f);

    // create circle
    const bl::rc::Color circleColor =
        node.getType() == world::Node::Path ? sf::Color::Green : sf::Color::Red;
    for (unsigned int ci = 0; ci < CircleTriangleCount; ++ci) {
        const unsigned int b = ci * 3;

        auto& center = vb[b];
        center.color = circleColor;
        center.pos   = {0.f, 0.f, 0.f};

        for (unsigned int j = 1; j < 3; ++j) {
            const unsigned int i = b + j;
            auto& v              = vb[i];
            const float d =
                static_cast<float>(ci + j - 1) / static_cast<float>(CircleTriangleCount) * 360.f;
            const float r = bl::math::degreesToRadians(d);
            v.color       = circleColor;
            v.pos.x       = std::cos(r) * Radius;
            v.pos.y       = std::sin(r) * Radius;
            v.pos.z       = 0.f;
        }
    }

    // render triangles
    for (unsigned int ai = 0; ai < ArrowCount; ++ai) {
        const unsigned int b = ai * ArrowTriangleCount * 3 + CircleVertexCount;
        const float degrees  = static_cast<float>(ai) * 30.f;
        const float d        = node.getDistanceToCover(degrees);
        const float f        = d / world::Node::MaxCoverDistance;
        const float r        = bl::math::degreesToRadians(degrees);
        const float c        = std::cos(r);
        const float s        = std::sin(r);

        const bl::rc::Color arrowColor = node.getType() == world::Node::Path ?
                                             sf::Color::Transparent :
                                             sf::Color(255.f * f, 255.f * (1.f - f), 20, 120);

        auto& p0 = vb[b];
        p0.color = arrowColor;
        p0.pos.x = d * c;
        p0.pos.y = d * s;
        p0.pos.z = 0.f;

        auto& p1 = vb[b + 1];
        p1.color = arrowColor;
        p1.pos.x = ArrowWidth * -s;
        p1.pos.y = ArrowWidth * c;
        p1.pos.z = 0.f;

        auto& p2 = vb[b + 2];
        p2.color = arrowColor;
        p2.pos.x = ArrowWidth * s;
        p2.pos.y = ArrowWidth * -c;
        p2.pos.z = 0.f;
    }

    vb.commit();
    vb.addToSceneWithCustomPipeline(engine->renderer().getObserver().getCurrentScene(),
                                    bl::rc::UpdateSpeed::Dynamic,
                                    bl::rc::Config::PipelineIds::Unlit2DGeometry);
}

void Render::init(bl::engine::Engine& e) { engine = &e; }

void Render::update(std::mutex&, float, float, float, float) {}

} // namespace sys
} // namespace core

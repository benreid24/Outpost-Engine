#include <Core/Systems/Render.hpp>

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Graphics/Circle.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/VertexBuffer2D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>

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

void Render::init(bl::engine::Engine& e) {
    engine = &e;

    VkPipelineRasterizationStateCreateInfo lineRasterizer{};
    lineRasterizer.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    lineRasterizer.depthClampEnable = VK_FALSE;
    lineRasterizer.rasterizerDiscardEnable = VK_FALSE;
    lineRasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    lineRasterizer.lineWidth =
        std::min(e.renderer().vulkanState().physicalDeviceProperties.limits.lineWidthRange[1], 3.f);
    lineRasterizer.cullMode        = VK_CULL_MODE_NONE;
    lineRasterizer.frontFace       = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    lineRasterizer.depthBiasEnable = VK_FALSE;

    e.renderer().pipelineCache().createPipline(
        NodeEdgesPipelineId,
        bl::rc::vk::PipelineParameters()
            .withShaders(bl::rc::Config::ShaderIds::Vertex2D,
                         bl::rc::Config::ShaderIds::Fragment2DUnlit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
            .withSimpleDepthStencil(false)
            .withRasterizer(lineRasterizer)
            .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
            .addDescriptorSet<bl::rc::ds::Object2DFactory>()
            .build());
}

void Render::updateTestGraphicsColor(bl::ecs::Entity entity, bl::rc::Color color) {
    bl::com::Shape2D* shape = engine->ecs().getComponent<bl::com::Shape2D>(entity);
    if (shape) {
        for (auto& v : shape->indexBuffer.vertices()) { v.color = color; }
        shape->indexBuffer.queueTransfer();
    }
}

void Render::update(std::mutex&, float, float, float, float) {}

} // namespace sys
} // namespace core

#include <Core/Systems/Render.hpp>

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Graphics/Circle.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/VertexBuffer2D.hpp>
#include <BLIB/Render/Config/ShaderIds.hpp>
#include <BLIB/Render/Descriptors/Builtin/GlobalDataFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/ShadowMapFactory.hpp>
#include <Core/Arena/Generation/Biome.hpp>
#include <Core/Rendering/PipelineIds.hpp>
#include <Core/Rendering/TerrainDescriptors.hpp>
#include <Core/Rendering/TerrainVertex.hpp>

namespace core
{
namespace sys
{
Render::Render()
: engine(nullptr)
, terrainBiomeMaterials{} {}

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
    lineRasterizer.lineWidth               = std::min(
        e.renderer().vulkanState().getPhysicalDeviceProperties().limits.lineWidthRange[1], 3.f);
    lineRasterizer.cullMode        = VK_CULL_MODE_NONE;
    lineRasterizer.frontFace       = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    lineRasterizer.depthBiasEnable = VK_FALSE;

    e.renderer().pipelineCache().createPipeline(
        NodeEdgesPipelineId,
        bl::rc::vk::PipelineParameters()
            .withShaders(bl::rc::cfg::ShaderIds::Vertex2D, bl::rc::cfg::ShaderIds::Fragment2DUnlit)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
            .withSimpleDepthStencil(false)
            .withRasterizer(lineRasterizer)
            .addDescriptorSet<bl::rc::dsi::Scene2DFactory>()
            .addDescriptorSet<bl::rc::dsi::Object2DFactory>()
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

void Render::setupTerrainRendering() {
    VkPipelineDepthStencilStateCreateInfo depthStencilDepthEnabled{};
    depthStencilDepthEnabled.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilDepthEnabled.depthTestEnable       = VK_TRUE;
    depthStencilDepthEnabled.depthWriteEnable      = VK_TRUE;
    depthStencilDepthEnabled.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilDepthEnabled.depthBoundsTestEnable = VK_FALSE;
    depthStencilDepthEnabled.minDepthBounds        = 0.0f; // Optional
    depthStencilDepthEnabled.maxDepthBounds        = 1.0f; // Optional
    depthStencilDepthEnabled.stencilTestEnable     = VK_FALSE;
    depthStencilDepthEnabled.front                 = {}; // Optional (Stencil)
    depthStencilDepthEnabled.back                  = {}; // Optional (Stencil)

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp          = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

    VkPipelineRasterizationStateCreateInfo rasterizerShadow = rasterizer;
    rasterizerShadow.cullMode                               = VK_CULL_MODE_FRONT_BIT;
    rasterizerShadow.depthBiasEnable                        = VK_TRUE;

    engine->renderer().pipelineCache().createPipeline(
        render::PipelineIds::SolidTerrain,
        bl::rc::vk::PipelineParameters()
            .withShaders("Resources/Shaders/terrainSolid.vert.spv",
                         "Resources/Shaders/terrainSolid.frag.spv")
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(render::TerrainVertex::bindingDescription(),
                              render::TerrainVertex::attributeDescriptions())
            .withRasterizer(rasterizer)
            .withDepthStencilState(&depthStencilDepthEnabled)
            .withBlendConfig(bl::rc::vk::BlendParameters().withSimpleColorBlendState(
                bl::rc::vk::BlendParameters::ColorBlendBehavior::Overwrite, 4))
            .addDescriptorSet<bl::rc::dsi::GlobalDataFactory>()
            .addDescriptorSet<bl::rc::dsi::Scene3DFactory>()
            .addDescriptorSet<render::terrain::BiomeInfoDescriptorSetFactory>()
            .build());

    engine->renderer().pipelineCache().createPipeline(
        render::PipelineIds::SolidTerrainShadowMap,
        bl::rc::vk::PipelineParameters()
            .withShader("Resources/Shaders/terrainSolidShadow.vert.spv", VK_SHADER_STAGE_VERTEX_BIT)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .withVertexFormat(render::TerrainVertex::bindingDescription(),
                              bl::rc::prim::Vertex3D::attributeDescriptionsPositionsOnly())
            .withRasterizer(rasterizerShadow)
            .addDynamicStates({VK_DYNAMIC_STATE_DEPTH_BIAS})
            .withBlendConfig(bl::rc::vk::BlendParameters().withSimpleColorBlendState(
                bl::rc::vk::BlendParameters::ColorBlendBehavior::None))
            .withDepthStencilState(&depthStencilDepthEnabled)
            .addDescriptorSet<bl::rc::dsi::ShadowMapFactory>()
            .build());

    engine->renderer().materialPipelineCache().createPipeline(
        render::MaterialPipelineIds::SolidTerrain,
        bl::rc::mat::MaterialPipelineSettings(
            render::PipelineIds::SolidTerrain) // TODO - forward pipeline?
            .withRenderPhasePipelineOverride(bl::rc::cfg::RenderPhases::Deferred,
                                             render::PipelineIds::SolidTerrain)
            .withRenderPhasePipelineOverride(bl::rc::cfg::RenderPhases::ShadowMap,
                                             render::PipelineIds::SolidTerrainShadowMap)
            // TODO - point shadow pipeline
            .build());

    auto biomeInfo = engine->renderer().getGlobalShaderResources().getShaderResourceWithKey(
        render::terrain::BiomeInfoShaderResourceKey);

    const auto setupBiomeInfo = [this, biomeInfo](arena::gen::Biome biome,
                                                  const char* texturePath) {
        auto textureAsset =
            engine->assets().getAssetFromSourcePath<bl::asi::TexturePayload>(texturePath);
        auto texture  = engine->renderer().texturePool().getOrLoadTexture(textureAsset);
        auto material = engine->renderer().materialPool().getOrCreateFromTexture(texture);
        biomeInfo->getBuffer()[0][static_cast<std::size_t>(biome)].materialIndex = material.getId();
        terrainBiomeMaterials[static_cast<std::size_t>(biome)]                   = material;
    };

    setupBiomeInfo(arena::gen::Biome::Desert, "Resources/Textures/Terrain/sand.png");
    setupBiomeInfo(arena::gen::Biome::Forest, "Resources/Textures/Terrain/forest.png");
    setupBiomeInfo(arena::gen::Biome::Grassland, "Resources/Textures/Terrain/grass.png");
    setupBiomeInfo(arena::gen::Biome::Mountain, "Resources/Textures/Terrain/rock.png");
    setupBiomeInfo(arena::gen::Biome::Snow, "Resources/Textures/Terrain/snow.png");
    setupBiomeInfo(arena::gen::Biome::Beach, "Resources/Textures/Terrain/sand.png");
    setupBiomeInfo(arena::gen::Biome::Water, "Resources/Textures/Terrain/water.png");
    setupBiomeInfo(arena::gen::Biome::River, "Resources/Textures/Terrain/water.png");
    biomeInfo->performTransfer();
}

} // namespace sys
} // namespace core

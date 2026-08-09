#include <Core/Rendering/TerrainVertex.hpp>

namespace core
{
namespace render
{
VkVertexInputBindingDescription TerrainVertex::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(TerrainVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 7> TerrainVertex::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 7> attributes{};
    auto baseAttributes = bl::rc::prim::Vertex3D::attributeDescriptions();
    std::copy(baseAttributes.begin(), baseAttributes.end(), attributes.begin());

    // biome indices
    attributes[5].binding  = 0;
    attributes[5].location = 5;
    attributes[5].format   = VK_FORMAT_R32G32B32A32_UINT;
    attributes[5].offset   = offsetof(TerrainVertex, biomeIndices);

    // biome weights
    attributes[6].binding  = 0;
    attributes[6].location = 6;
    attributes[6].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributes[6].offset   = offsetof(TerrainVertex, biomeWeights);

    return attributes;
}

} // namespace render
} // namespace core

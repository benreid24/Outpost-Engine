#ifndef CORE_RENDERING_TERRAINVERTEX_HPP
#define CORE_RENDERING_TERRAINVERTEX_HPP

#include <BLIB/Render/Primitives/Vertex3D.hpp>
#include <glm/glm.hpp>

/**
 * @addtogroup Rendering
 * @ingroup Core
 * @brief Collection of renderering specific code and functionality
 */

namespace core
{
namespace render
{
/**
 * @brief Vertex type used for terrain rendering
 *
 * @ingroup Rendering
 */
struct TerrainVertex : public bl::rc::prim::Vertex3D {
    glm::u32vec4 biomeIndices;
    glm::vec4 biomeWeights;

    /**
     * @brief Returns the vertex binding description for the renderer
     *
     * @return VkVertexInputBindingDescription The vertex binding description
     */
    static VkVertexInputBindingDescription bindingDescription();

    /**
     * @brief Returns the attribute descriptions for each vertex attribute
     *
     * @return Descriptions for each attribute
     */
    static std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions();
};

} // namespace render
} // namespace core

#endif

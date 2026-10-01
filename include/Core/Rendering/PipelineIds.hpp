#ifndef CORE_RENDERING_PIPELINEIDS_HPP
#define CORE_RENDERING_PIPELINEIDS_HPP

#include <cstdint>

namespace core
{
namespace render
{
/**
 * @brief Ids of rendering pipelines
 *
 * @ingroup Rendering
 */
struct PipelineIds {
    static constexpr std::uint32_t SolidTerrain          = 5000;
    static constexpr std::uint32_t SolidTerrainShadowMap = 5001;
};

/**
 * @brief Ids of material pipelines
 *
 * @ingroup Rendering
 */
struct MaterialPipelineIds {
    static constexpr std::uint32_t SolidTerrain = 5000;
};

} // namespace render
} // namespace core

#endif

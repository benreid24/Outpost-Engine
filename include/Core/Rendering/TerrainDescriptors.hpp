#ifndef CORE_RENDERING_TERRAINDESCRIPTORS_HPP
#define CORE_RENDERING_TERRAINDESCRIPTORS_HPP

#include <BLIB/Render/Buffers/BufferSingleDeviceLocalSourced.hpp>
#include <BLIB/Render/Descriptors/Generic/BufferBinding.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <Core/Arena/Generation/Biome.hpp>
#include <array>

namespace core
{
/// Contains classes, descriptors, shader resources, and other renderer specific code
namespace render
{
/// Contains renderer specific code for terrain rendering
namespace terrain
{
/**
 * @brief Information for a biome used by the terrain shaders
 *
 * @ingroup Rendering
 */
struct BiomeInfo {
    std::uint32_t materialIndex;
};

namespace detail
{
using BiomeInfoShaderResource = bl::rc::sr::BufferShaderResource<
    bl::rc::buf::BufferSingleDeviceLocalSourced<
        std::array<BiomeInfo, static_cast<std::size_t>(arena::gen::Biome::COUNT)>,
        bl::rc::buf::Role::UBO>,
    1>;
}

/**
 * @brief The resource key to access the biome info buffer
 *
 * @ingroup Rendering
 */
constexpr bl::rc::sr::Key<detail::BiomeInfoShaderResource> BiomeInfoShaderResourceKey{
    "core_BiomeInfoShaderResource"};

/// Implementation details
namespace detail
{
using BiomeInfoBinding =
    bl::rc::ds::BufferBinding<BiomeInfoShaderResource, BiomeInfoShaderResourceKey,
                              bl::rc::sr::StoreKey::Global, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER>;

using BiomeInfoDescriptorSetBindings = bl::rc::ds::Bindings<BiomeInfoBinding>;
} // namespace detail

/**
 * @brief The descriptor set instance for terrain biome information
 *
 * @ingroup Rendering
 */
using BiomeInfoDescriptorSetInstance =
    bl::rc::ds::GenericDescriptorSetInstance<detail::BiomeInfoDescriptorSetBindings>;

/**
 * @brief The descriptor set factory for terrain biome information
 *
 * @ingroup Rendering
 */
using BiomeInfoDescriptorSetFactory =
    bl::rc::ds::GenericDescriptorSetFactory<detail::BiomeInfoDescriptorSetBindings,
                                            VK_SHADER_STAGE_FRAGMENT_BIT>;

} // namespace terrain
} // namespace render
} // namespace core

#endif

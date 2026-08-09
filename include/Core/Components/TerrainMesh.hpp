#ifndef CORE_COMPONENTS_TERRAINMESH_HPP
#define CORE_COMPONENTS_TERRAINMESH_HPP

#include <BLIB/Components/Mesh.hpp>
#include <Core/Rendering/PipelineIds.hpp>
#include <Core/Rendering/TerrainVertex.hpp>

namespace core
{
namespace com
{
/**
 * @brief Component containing a mesh for terrain rendering
 *
 * @ingroup Components
 */
using TerrainMesh = bl::com::Mesh<render::TerrainVertex, render::MaterialPipelineIds::SolidTerrain>;

} // namespace com
} // namespace core

#endif

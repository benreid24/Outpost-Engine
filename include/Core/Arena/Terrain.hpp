#ifndef CORE_ARENA_TERRAIN_HPP
#define CORE_ARENA_TERRAIN_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Graphics/Terrain.hpp>
#include <Core/Arena/Generation/PerlinSampler.hpp>
#include <Core/Arena/Generation/Seed.hpp>
#include <Core/Arena/Node.hpp>
#include <Core/Components/TerrainMesh.hpp>

namespace core
{
namespace arena
{
namespace gen
{
class Generator;
}
/**
 * @brief Representation of the terrain in an arena
 *
 * @ingroup Arena
 */
class Terrain {
public:
    /**
     * @brief Represents a biome sampled from the terrain with its weight
     */
    struct SampledBiome {
        gen::Biome biome;
        float height;
        float weight;
    };

    /**
     * @brief Creates the terrain with no data
     */
    Terrain();

    /**
     * @brief Creates engine objects for the terrain and adds them to the world
     *
     * @param world The world to create the objects in
     * @param step The step size for the terrain
     */
    void addToWorld(bl::engine::World& world, float step);

    /**
     * @brief Returns the height of the terrain at the given position
     *
     * @param position The world position to sample the height at
     * @return The height of the terrain at the given position
     */
    float sampleHeight(const glm::vec2& position) const;

    /**
     * @brief Samples the biomes at the given world position
     *
     * @param position The world position to sample the biomes at
     * @return A static vector containing the biomes at the given position
     */
    bl::ctr::StaticVector<SampledBiome, 4> sampleBiomes(const glm::vec2& position) const;

    /**
     * @brief Returns the node index of the given world position
     *
     * @param worldPos The position to get the index for in world coordinates
     * @return The index of the given world position
     */
    glm::u32vec2 worldPosToIndex(const glm::vec2& worldPos) const;

private:
    glm::vec2 worldSize;
    float maxHeight;
    bl::ctr::Vector2D<Node> nodes;
    gen::PerlinSampler heightSampler;
    bl::gfx::Terrain<com::TerrainMesh> terrainDrawable;
    float step;

    void postprocess();
    std::array<std::pair<glm::u32vec2, float>, 4> getPositionWeights(const glm::vec2& pos) const;

    friend class gen::Generator;
};

} // namespace arena
} // namespace core

#endif

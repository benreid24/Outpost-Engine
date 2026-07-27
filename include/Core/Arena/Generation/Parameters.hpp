#ifndef CORE_ARENA_GENERATION_PARAMETERS_HPP
#define CORE_ARENA_GENERATION_PARAMETERS_HPP

#include <Core/Arena/Generation/RuledBiome.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace core
{
namespace arena
{
/// Contains methods and classes for arena generation
namespace gen
{
/**
 * @brief Arena and terrain generation parameters
 *
 * @ingroup Arena
 */
struct Parameters {
    /**
     * @brief Parameters specific to the Perlin noise generator
     */
    struct PerlinParameters {
        float frequency;
        unsigned int octaves;
        float persistence;

        /**
         * @brief Sets sane defaults
         */
        PerlinParameters()
        : frequency(0.01f)
        , octaves(16)
        , persistence(0.5f) {}
    };

    glm::vec2 worldSize;
    float worldStep;
    float maxHeight;
    float waterHeight;
    PerlinParameters terrainPerlin;
    PerlinParameters moisturePerlin;
    std::vector<RuledBiome> biomes;

    /**
     * @brief Initializes the parameters with sane defaults
     *
     * @param worldSize The size of the world in world coordinates
     * @param step The step size to generate terrain with in world coordinates
     * @param maxHeight The maximum height of the terrain in world coordinates
     */
    Parameters(const glm::vec2& worldSize, float step, float maxHeight)
    : worldSize(worldSize)
    , worldStep(step)
    , maxHeight(maxHeight)
    , waterHeight(maxHeight * 0.45f)
    , terrainPerlin()
    , moisturePerlin() {}

    /**
     * @brief Computes and returns the number of nodes to generate with
     */
    glm::u32vec2 getWorldNodeCount() const {
        return glm::u32vec2(std::ceil(worldSize.x / worldStep) + 0.01f,
                            std::ceil(worldSize.y / worldStep) + 0.01f);
    }
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

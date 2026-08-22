#ifndef CORE_ARENA_GENERATION_PERLINSAMPLER_HPP
#define CORE_ARENA_GENERATION_PERLINSAMPLER_HPP

#include <BLIB/Util/Perlin.hpp>
#include <Core/Arena/Generation/Parameters.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
namespace gen
{
class Seed;

/**
 * @brief Helper class to encode perlin parameters for easy sampling
 *
 * @ingroup Arena
 */
class PerlinSampler {
public:
    /**
     * @brief Creates an empty sampler
     */
    PerlinSampler() = default;

    /**
     * @brief Creates a sampler with the given parameters and origin
     *
     * @param params The perlin noise parameters
     * @param origin The origin in world coordinates
     * @param seed The seed to use
     * @param id The perlin noise id to use
     */
    PerlinSampler(const Parameters::PerlinParameters& params, const glm::vec2& origin,
                  const Seed& seed, std::uint64_t id = 0);

    /**
     * @brief Copies the sampler
     *
     * @param copy The sampler to copy
     */
    PerlinSampler(const PerlinSampler& copy) = default;

    /**
     * @brief Samples the perlin noise at the given position
     *
     * @param pos The position in world coordinates to sample at
     * @return The perlin noise value in range [0, 1]
     */
    float sample(const glm::vec2& pos) const;

private:
    bl::util::Perlin<float> perlin;
    glm::vec2 origin;
    Parameters::PerlinParameters params;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

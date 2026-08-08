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

class PerlinSampler {
public:
    PerlinSampler() = default;

    PerlinSampler(const Parameters::PerlinParameters& params, const glm::vec2& origin,
                  const Seed& seed, std::uint64_t id = 0);

    PerlinSampler(const PerlinSampler& copy) = default;

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

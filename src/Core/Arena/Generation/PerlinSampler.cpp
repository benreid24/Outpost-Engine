#include <Core/Arena/Generation/PerlinSampler.hpp>

#include <Core/Arena/Generation/Seed.hpp>

namespace core
{
namespace arena
{
namespace gen
{
PerlinSampler::PerlinSampler(const Parameters::PerlinParameters& params, const glm::vec2& origin,
                             const Seed& seed, std::uint64_t id)
: perlin(seed.getPerlin(id))
, origin(origin)
, params(params) {}

float PerlinSampler::sample(const glm::vec2& pos) const {
    const float xf = pos.x - origin.x;
    const float yf = pos.y - origin.y;
    float normal   = perlin.octave2DNormalized(
        xf * params.frequency, yf * params.frequency, params.octaves, params.persistence);
    normal = (normal + 1.f) * 0.5f; // map to [0,1]
    return normal;
}

} // namespace gen
} // namespace arena
} // namespace core

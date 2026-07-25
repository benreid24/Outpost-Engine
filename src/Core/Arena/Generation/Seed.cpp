#include <Core/Arena/Generation/Seed.hpp>

#include <BLIB/Util/Random.hpp>
#include <numeric>

namespace core
{
namespace arena
{
namespace gen
{
Seed::Seed()
: Seed(bl::util::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max())) {}

Seed::Seed(std::uint64_t seed)
: seed(seed)
, perlin(seed)
, rng(seed) {}

void Seed::reseed(std::uint64_t seed) {
    this->seed = seed;
    perlin.reseed(seed);
    rng.seed(seed);
}

float Seed::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

} // namespace gen
} // namespace arena
} // namespace core

#include <Core/Arena/Generation/Seed.hpp>

#include <BLIB/Random/Random.hpp>
#include <numeric>

namespace core
{
namespace arena
{
namespace gen
{
Seed::Seed()
: Seed(bl::rand::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max())) {}

Seed::Seed(std::uint64_t seed)
: seed(seed)
, rng(seed) {}

void Seed::reseed(std::uint64_t seed) {
    this->seed = seed;
    rng.seed(seed);
}

bl::rand::Perlin<float> Seed::getPerlin(std::uint64_t offset) const {
    bl::rand::Perlin<float> perlin(seed + offset);
    return perlin;
}

float Seed::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

std::uint64_t Seed::getUint64(std::uint64_t min, std::uint64_t max) {
    std::uniform_int_distribution<std::uint64_t> dist(min, max);
    return dist(rng);
}

} // namespace gen
} // namespace arena
} // namespace core

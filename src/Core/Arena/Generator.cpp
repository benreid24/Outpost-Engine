#include <Core/Arena/Generator.hpp>

#include <BLIB/Util/Random.hpp>
#include <numeric>

namespace core
{
namespace arena
{
Generator::Generator()
: Generator(bl::util::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max())) {}

Generator::Generator(std::uint64_t seed)
: seed(seed)
, perlin(seed)
, rng(seed) {}

void Generator::reseed(std::uint64_t seed) {
    this->seed = seed;
    perlin.reseed(seed);
    rng.seed(seed);
}

float Generator::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

} // namespace arena
} // namespace core

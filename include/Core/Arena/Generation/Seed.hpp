#ifndef CORE_ARENA_GENERATION_SEED_HPP
#define CORE_ARENA_GENERATION_SEED_HPP

#include <BLIB/Util/Perlin.hpp>
#include <cstdint>
#include <random>

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Random number generator for world generation
 *
 * @ingroup Arena
 */
class Seed {
public:
    /**
     * @brief Creates the generator with a random seed
     */
    Seed();

    /**
     * @brief Creates the generator with the given seed
     *
     * @param seed The seed to use
     */
    Seed(std::uint64_t seed);

    /**
     * @brief Sets the seed to use
     *
     * @param seed The seed to use
     */
    void reseed(std::uint64_t seed);

    /**
     * @brief Gets the current seed
     */
    std::uint64_t getSeed() const { return seed; }

    /**
     * @brief Returns a Perlin noise generator seeded with the current seed
     */
    const bl::util::Perlin<float>& getPerlin() const { return perlin; }

    /**
     * @brief Returns a random number in the given range of a float type
     *
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return A random number in the range [min,max]
     */
    float getFloat(float min, float max);

private:
    std::uint64_t seed;
    bl::util::Perlin<float> perlin;
    std::mt19937_64 rng;
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

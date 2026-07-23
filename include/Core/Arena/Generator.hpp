#ifndef CORE_ARENA_GENERATOR_HPP
#define CORE_ARENA_GENERATOR_HPP

#include <BLIB/Util/Perlin.hpp>
#include <cstdint>

namespace core
{
namespace arena
{
/**
 * @brief Random number generator for world generation
 *
 * @ingroup Arena
 */
class Generator {
public:
    /**
     * @brief Creates the generator with a random seed
     */
    Generator();

    /**
     * @brief Creates the generator with the given seed
     *
     * @param seed The seed to use
     */
    Generator(std::uint64_t seed);

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

private:
    std::uint64_t seed;
    bl::util::Perlin<float> perlin;
};

} // namespace arena
} // namespace core

#endif

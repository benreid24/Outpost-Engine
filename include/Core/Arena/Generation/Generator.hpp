#ifndef CORE_ARENA_GENERATION_GENERATOR_HPP
#define CORE_ARENA_GENERATION_GENERATOR_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <Core/Arena/Generation/Parameters.hpp>
#include <Core/Arena/Generation/Seed.hpp>

namespace core
{
namespace arena
{
class Arena;

namespace gen
{
class ProtoTerrain;

/**
 * @brief Terrain and arena generator
 *
 * @ingroup Arena
 */
class Generator {
public:
    /**
     * @brief Initializes the generator
     *
     * @param seed The seed to use for generation
     * @param parameters The generation parameters
     */
    Generator(std::uint64_t seed, const Parameters& parameters);

    /**
     * @brief Returns the seed being used
     */
    const Seed& getSeed() const { return seed; }

    /**
     * @brief Returns the generation parameters being used
     */
    const Parameters& getParameters() const { return params; }

    /**
     * @brief Returns the raw terrain heightmap used for generation
     */
    const bl::ctr::Vector2D<float>& getHeightmap() const { return heightmap; }

    /**
     * @brief Returns the raw terrain moisture map used for generation
     */
    const bl::ctr::Vector2D<float>& getMoistureMap() const { return moistureMap; }

    /**
     * @brief Runs arena generation and populates the given Arena
     * @param output
     */
    void generate(Arena& output);

private:
    Seed seed;
    Parameters params;
    bl::ctr::Vector2D<float> heightmap;
    bl::ctr::Vector2D<float> moistureMap;

    void raiseLakes(ProtoTerrain& source);
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

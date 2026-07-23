#ifndef CORE_ARENA_ARENA_HPP
#define CORE_ARENA_ARENA_HPP

#include <Core/Arena/Terrain.hpp>

/**
 * @addtogroup Arena
 * @ingroup Core
 * @brief Data representation and simulation of in game arenas
 */

namespace core
{
namespace arena
{
/**
 * @brief Represents a single arena in game with terrain, units, tracks, and other data
 *
 * @ingroup Arena
 */
class Arena {
public:
    /**
     * @brief Creates an empty arena
     */
    Arena();

    /**
     * @brief Generates the arena using the given generator and size
     *
     * @param seed The seed to use for terrain generation
     * @param size The size of the arena to generate in world coordinates
     * @param maxHeight The maximum height of the terrain to generate
     */
    void generate(std::uint64_t seed, const glm::vec2& size, float maxHeight);

    /**
     * @brief Adds the arena to the given world
     *
     * @param world The world to add the arena to
     */
    void addToWorld(bl::engine::World& world);

private:
    Generator generator;
    Terrain terrain;
};

} // namespace arena
} // namespace core

#endif

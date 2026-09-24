#ifndef CORE_ARENA_ARENA_HPP
#define CORE_ARENA_ARENA_HPP

#include <Core/Arena/Terrain.hpp>
#include <Core/Arena/Train/Track.hpp>
#include <Core/Arena/Train/Train.hpp>

/**
 * @addtogroup Arena
 * @ingroup Core
 * @brief Data representation and simulation of in game arenas
 */

namespace core
{
namespace arena
{
namespace gen
{
class Generator;
}

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

    /**
     * @brief Returns the terrain of the arena
     */
    const Terrain& getTerrain() const { return terrain; }

    /**
     * @brief Returns the train track of the arena
     */
    const train::Track& getTrack() const { return track; }

    /**
     * @brief Updates per frame arena logic
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

private:
    gen::Seed generator;
    Terrain terrain;
    train::Track track;
    train::Train train;
    bl::util::ThreadPool* threadPool;

    friend class gen::Generator;
};

} // namespace arena
} // namespace core

#endif

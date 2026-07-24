#ifndef CORE_ARENA_GENERATION_BIOME_HPP
#define CORE_ARENA_GENERATION_BIOME_HPP

namespace core
{
namespace arena
{
namespace gen
{
/**
 * @brief Represents the biome of a node
 *
 * @ingroup Arena
 */
enum struct Biome {
    Desert    = 0,
    Forest    = 1,
    Grassland = 2,
    Mountain  = 3,
    Snow      = 4,
    Beach     = 5,
    Water     = 6,
    COUNT
};

} // namespace gen
} // namespace arena
} // namespace core

#endif

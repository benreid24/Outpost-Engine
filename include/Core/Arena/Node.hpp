#ifndef CORE_ARENA_NODE_HPP
#define CORE_ARENA_NODE_HPP

#include <Core/Arena/Generation/Biome.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace arena
{
/**
 * @brief Represents a single node in the arena terrain
 *
 * @ingroup Arena
 */
struct Node {
    glm::u32vec2 index;
    glm::vec2 worldPos;
    gen::Biome biome;
};

} // namespace arena
} // namespace core

#endif

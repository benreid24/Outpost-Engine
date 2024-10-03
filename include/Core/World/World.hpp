#ifndef CORE_WORLD_WORLD_HPP
#define CORE_WORLD_WORLD_HPP

#include <BLIB/Engine/Worlds/World2D.hpp>
#include <Core/World/Cover.hpp>
#include <Core/World/Node.hpp>
#include <vector>

/**
 * @addtogroup World
 * @ingroup Core
 * @brief Collection of classes for the in-game world
 */

namespace core
{
/// Collection of classes for the in-game world
namespace world
{
/**
 * @brief The game world class
 *
 * @ingroup World
 */
class World : public bl::engine::World2D {
public:
    // TODO - interface

private:
    std::vector<Cover> covers;
    std::vector<Node> nodes;
    // TODO - spatial partitions?
};

} // namespace world
} // namespace core

#endif

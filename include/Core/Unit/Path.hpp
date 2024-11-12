#ifndef CORE_UNIT_PATH_HPP
#define CORE_UNIT_PATH_HPP

#include <BLIB/Util/HashCombine.hpp>
#include <Core/World/Node.hpp>

namespace core
{
namespace unit
{
/**
 * @brief Data structure representing a path a unit can take
 *
 * @ingroup Unit
 */
struct Path {
public:
    /**
     * @brief Waypoint along a path
     */
    struct Waypoint {
        glm::vec2 position;
        const world::Node* worldNode;

        /**
         * @brief Creates an empty waypoint
         */
        Waypoint()
        : position()
        , worldNode(nullptr) {}

        /**
         * @brief Creates a waypoint at the given position and optional world node
         *
         * @param position The position of the waypoint
         * @param worldNode The world node of the waypoint
         */
        Waypoint(const glm::vec2& position, const world::Node* worldNode = nullptr)
        : position(position)
        , worldNode(worldNode) {}

        /**
         * @brief Tests whether this waypoint is equal to another by direct position equality
         *
         * @param other The waypoint to test equality with
         * @return True if the two waypoints have the same position, false otherwise
         */
        bool operator==(const Waypoint& other) const { return position == other.position; }

        /**
         * @brief Tests whether this waypoint is equal to another by direct position equality
         *
         * @param other The waypoint to test equality with
         * @return True if the two waypoints have different positions, false otherwise
         */
        bool operator!=(const Waypoint& other) const { return position != other.position; }
    };

    /**
     * @brief Hash functor for path waypoints
     */
    struct WaypointHash {
        std::size_t operator()(const Waypoint& wp) const {
            std::hash<float> hasher;
            return bl::util::hashCombine(hasher(wp.position.x), hasher(wp.position.y));
        }
    };

    std::vector<Waypoint> waypoints;
};

} // namespace unit
} // namespace core

#endif

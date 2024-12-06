#ifndef CORE_WORLD_COLLISIONS_HPP
#define CORE_WORLD_COLLISIONS_HPP

#include <box2d/box2d.h>
#include <cstdint>
#include <limits>

namespace core
{
namespace world
{
/**
 * @brief Contains Box2d configuration for world collisions
 *
 * @ingroup World
 */
struct Collisions {
    /**
     * @brief Object categories used for collision and query filtering
     */
    enum Category : std::uint32_t {
        /// World geometry that blocks movement
        ShortCover = 0x1 << 0,

        /// World geometry that blocks movement and bullets
        TallCover = 0x1 << 1,

        /// Units that move around and can be shot
        Unit = 0x1 << 2,

        /// Buildings that can be targeted
        Building = 0x1 << 3,

        /// Bullets that impact units and cover
        Bullet = 0x1 << 4,

        /// Virtual category for world queries
        CoverQueryRay = 0x1 << 5,

        /// Sensors that do not interact with bullets or cover
        WorldSensor = 0x1 << 6,

        /// Virtual category for sensor connection queries
        SensorQueryRay = 0x1 << 7,

        /// Helper value for masks that should collide with all categories
        All = std::numeric_limits<std::uint32_t>::max()
    };

    /**
     * @brief Returns the filter to use for world geometry
     */
    static b2Filter getShortCoverFilter();

    /**
     * @brief Returns the filter to use for world geometry
     */
    static b2Filter getTallCoverFilter();

    /**
     * @brief Returns the filter to use for units
     */
    static b2Filter getUnitFilter();

    /**
     * @brief Returns the filter to use for buildings
     */
    static b2Filter getBuildingFilter();

    /**
     * @brief Returns the filter to use for bullets
     */
    static b2Filter getBulletFilter();

    /**
     * @brief Returns the filter to use for sensors
     */
    static b2Filter getSensorFilter();

    /**
     * @brief Returns the filter to use for world cover queries
     */
    static b2QueryFilter getCoverQueryFilter();

    /**
     * @brief Returns the filter to use for world node connection queries
     */
    static b2QueryFilter getNodeConnectionFilter();

    /**
     * @brief Returns the filter to use when querying for units
     */
    static b2QueryFilter getUnitQueryFilter();

    /**
     * @brief Returns the filter to use when querying for buildings
     */
    static b2QueryFilter getBuildingQueryFilter();

    /**
     * @brief Returns the filter to use to test line of sight to units or buildings
     */
    static b2QueryFilter getLineOfSightQueryFilter();

    /**
     * @brief Returns the filter to use when querying for target-able entities
     */
    static b2QueryFilter getTargetQueryFilter();

    /**
     * @brief Returns the filter to use when querying for unit movement to nodes
     */
    static b2QueryFilter getUnitMovementQueryFilter();
};

} // namespace world
} // namespace core

#endif

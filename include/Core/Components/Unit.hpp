#ifndef CORE_COMPONENTS_UNIT_HPP
#define CORE_COMPONENTS_UNIT_HPP

#include <BLIB/Components/Physics2D.hpp>
#include <Core/Commands/Queue.hpp>
#include <Core/Commands/UnitCommand.hpp>
#include <Core/Factions/FactionId.hpp>
#include <Core/Unit/Capabilities.hpp>

/**
 * @addtogroup Unit
 * @ingroup Core
 * @brief Classes and behaviors for in-game units
 */

namespace core
{
namespace sys
{
class Unit;
}
namespace com
{
/**
 * @brief Component and data storage for in game units
 *
 * @ingroup Components
 * @ingroup Unit
 */
class Unit {
public:
    /**
     * @brief Creates a unit that cannot move or shoot
     *
     * @param faction The id of the faction that this unit belongs to
     * @param physics The physics component of the unit
     */
    Unit(fcn::FactionId faction, bl::com::Physics2D& physics);

    /**
     * @brief Returns the entity id of this unit
     */
    bl::ecs::Entity getId() const { return physics.getOwner(); }

    /**
     * @brief Returns the position of the unit in world coordinates
     */
    glm::vec2 getPosition() const { return physics.getTransform().getGlobalPosition(); }

    /**
     * @brief Returns the rotation of the unit in degrees
     */
    float getRotation() const { return physics.getTransform().getRotation(); }

    /**
     * @brief Returns the id of the faction the unit belongs to
     */
    fcn::FactionId getFaction() const { return faction; }

    /**
     * @brief Returns the units capabilities
     */
    unit::Capabilities& capabilities() { return abilities; }

private:
    bl::com::Physics2D& physics;

    // unit data and components
    fcn::FactionId faction;
    unit::Capabilities abilities;

    friend class sys::Unit;
};

} // namespace com
} // namespace core

#endif

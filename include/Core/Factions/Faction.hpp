#ifndef CORE_FACTIONS_FACTION_HPP
#define CORE_FACTIONS_FACTION_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Factions/FactionId.hpp>
#include <Core/Factions/Filter.hpp>
#include <span>
#include <string>
#include <vector>

namespace core
{
namespace com
{
class Unit;
}

namespace fcn
{
class Factions;

/**
 * @brief A faction in the game. Units in a faction are allied with each other
 *
 * @ingroup Factions
 */
class Faction : private bl::util::NonCopyable {
public:
    /**
     * @brief Returns the id of this faction
     */
    FactionId getId() const { return id; }

    /**
     * @brief Returns the name of the factions
     */
    const std::string& getName() const { return name; }

    /**
     * @brief Sets the name of the faction
     *
     * @param name The new name
     */
    void setName(std::string_view name);

    /**
     * @brief Returns the list of units in this factions
     */
    const std::vector<com::Unit*>& getUnits() { return units; }

    /**
     * @brief Returns whether this fraction is allied with another
     *
     * @param other The other faction
     * @return True if the two factions are allied, false otherwise
     */
    bool isFriendly(FactionId other) const { return friendlyTo.matches(other); }

    /**
     * @brief Returns whether this fraction is allied with another
     *
     * @param other The other faction
     * @return True if the two factions are allied, false otherwise
     */
    bool isFriendly(const Faction& other) const { return friendlyTo.matches(other.id); }

    /**
     * @brief Returns whether this fraction is hostile with another
     *
     * @param other The other faction
     * @return True if the two factions are hostile, false otherwise
     */
    bool isHostile(FactionId other) const { return hostileTo.matches(other); }

    /**
     * @brief Returns whether this fraction is hostile with another
     *
     * @param other The other faction
     * @return True if the two factions are hostile, false otherwise
     */
    bool isHostile(const Faction& other) const { return hostileTo.matches(other.id); }

    /**
     * @brief Returns whether this fraction is neutral to another
     *
     * @param other The other faction
     * @return True if the two factions are neutral, false otherwise
     */
    bool isNeutral(FactionId other) const { return !isHostile(other) && !isFriendly(other); }

    /**
     * @brief Returns whether this fraction is neutral to another
     *
     * @param other The other faction
     * @return True if the two factions are neutral, false otherwise
     */
    bool isNeutral(const Faction& other) const { return !isHostile(other) && !isFriendly(other); }

private:
    const FactionId id;
    std::string name;
    Filter friendlyTo;
    Filter hostileTo;
    std::vector<com::Unit*> units;

    Faction(FactionId id);
    void addUnit(com::Unit* unit);
    void removeUnit(const com::Unit* unit);

    friend class Factions;
};

} // namespace fcn
} // namespace core

#endif

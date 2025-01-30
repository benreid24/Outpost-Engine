#include <Core/Factions/Faction.hpp>

namespace core
{
namespace fcn
{
Faction::Faction(FactionId id)
: id(id)
, friendlyTo(FilterEmpty)
, hostileTo(FilterEmpty) {
    units.reserve(32);
}

void Faction::addUnit(com::Unit* unit) { units.emplace_back(unit); }

void Faction::removeUnit(const com::Unit* unit) {
    for (auto it = units.begin(); it != units.end(); ++it) {
        if (*it == unit) {
            units.erase(it);
            break;
        }
    }
}

void Faction::setName(std::string_view n) { name = n; }

} // namespace fcn
} // namespace core

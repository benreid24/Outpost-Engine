#include <Core/Factions/Factions.hpp>

namespace core
{
namespace fcn
{
Factions::Factions()
: freeIds(FactionId::MaxFactions) {
    factions.resize(FactionId::MaxFactions);
}

Factions::~Factions() {
    for (FactionId::IdType i = 0; i < FactionId::MaxFactions; ++i) {
        if (freeIds.isAllocated(i)) { factions[i].destroy(); }
    }
}

Faction* Factions::createFaction(std::string_view name) {
    // TODO - allocate id, create faction, set name
    // TODO - emplace helper
    return nullptr;
}

void Factions::destroyFaction(FactionId id) {
    // TODO - destroy the faction. validate destroy is clean (no units)
}

void Factions::observe(const bl::ecs::event::ComponentAdded<com::Unit>& event) {
    // TODO - add to faction
}

void Factions::observe(const bl::ecs::event::ComponentRemoved<com::Unit>& event) {
    // TODO - add to faction
}

} // namespace fcn
} // namespace core

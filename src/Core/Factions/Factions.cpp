#include <Core/Factions/Factions.hpp>

#include <BLIB/Util/EmplaceHelper.hpp>

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
    if (!freeIds.available()) {
        BL_LOG_ERROR << "Ran out of faction ids";
        return nullptr;
    }

    const FactionId::IdType index = freeIds.allocate();
    const FactionId newId(0x1 << index);
    auto& factionSlot = factions[index];
    factionSlot.emplace(bl::util::EmplaceHelper([newId]() { return Faction(newId); }));
    Faction& faction = factionSlot.get();
    faction.setName(name);
    return &faction;
}

Faction* Factions::getFaction(FactionId id) {
    if (!factionExists(id)) { return nullptr; }
    return &factions[id.getIndex()].get();
}

void Factions::enumerateFactions(std::vector<Faction*>& result) {
    result.reserve(factions.size());
    result.clear();
    for (FactionId::IdType i = 0; i < factions.size(); ++i) {
        if (freeIds.isAllocated(i)) { result.emplace_back(&factions[i].get()); }
    }
}

void Factions::destroyFaction(FactionId id) {
    const auto i = id.getIndex();
    if (!freeIds.isAllocated(i)) {
        BL_LOG_WARN << "Double free of id: " << id;
        return;
    }

    Faction& faction = factions[i].get();
    if (!faction.getUnits().empty()) {
        BL_LOG_ERROR << "Trying to free faction " << id << " with " << faction.getUnits().size()
                     << " members remaining";
        return;
    }

    factions[i].destroy();
    freeIds.release(i);
}

void Factions::observe(const bl::ecs::event::ComponentAdded<com::Unit>& event) {
    const auto i = event.component.getFaction().getIndex();
    if (!freeIds.isAllocated(i)) {
        BL_LOG_ERROR << "Unit " << event.entity
                     << " added to invalid faction : " << event.component.getFaction();
        return;
    }

    factions[i].get().addUnit(&event.component);
}

void Factions::observe(const bl::ecs::event::ComponentRemoved<com::Unit>& event) {
    const auto i = event.component.getFaction().getIndex();
    if (!freeIds.isAllocated(i)) {
        BL_LOG_WARN << "Unit " << event.entity
                    << " removed from invalid faction : " << event.component.getFaction();
        return;
    }

    factions[i].get().removeUnit(&event.component);
}

} // namespace fcn
} // namespace core

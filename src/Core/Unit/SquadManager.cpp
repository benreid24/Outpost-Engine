#include <Core/Unit/SquadManager.hpp>

namespace core
{
namespace unit
{
SquadManager::SquadManager() { internalRefs.reserve(64); }

SquadManager::Ref SquadManager::makeSquad(fcn::FactionId faction) {
    Ref squad = squads.emplace(faction);
    internalRefs.emplace_back(squad);
    return squad;
}

std::vector<SquadManager::Ref> SquadManager::getFactionSquads(fcn::FactionId faction) {
    std::vector<SquadManager::Ref> result;
    std::size_t count;
    for (const auto& squad : internalRefs) {
        if (squad->getFaction() == faction) { ++count; }
    }
    result.reserve(count);
    for (auto& squad : internalRefs) {
        if (squad->getFaction() == faction) { result.emplace_back(squad); }
    }
    return result;
}

void SquadManager::init(bl::engine::Engine&) {
    // noop
}

void SquadManager::update(std::mutex&, float dt, float, float, float) {
    std::erase_if(internalRefs, [](auto& squad) { return squad.refCount() == 1; });

    for (auto& squad : internalRefs) {
        // TODO - squad AI
    }
}

} // namespace unit
} // namespace core

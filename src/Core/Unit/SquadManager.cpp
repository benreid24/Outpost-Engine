#include <Core/Unit/SquadManager.hpp>

#include <Core/Game.hpp>

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
    std::size_t count = 0;
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
        if (squad->commandQueue.hasCommand()) {
            if (squad->units.empty()) {
                squad->commandQueue.failCurrentCommand();
                continue;
            }

            if (squad->unitCommands.empty()) { issueUnitCommands(*squad); }
            else { checkUnitCommands(*squad); }
        }
    }
}

void SquadManager::issueUnitCommands(Squad& squad) {
    auto& game      = bl::game::Game::getInstance<Game>();
    const auto& cmd = squad.commandQueue.getCurrentCommand();

    using T = cmd::SquadCommand::Type;
    switch (cmd.getType()) {
    case T::Idle:
        doCommandCreateAndIssue(squad, [&game, &cmd]() {
            return game.commandStore().unitMakeIdle(cmd.getAggroLevel());
        });
        break;

    case T::Move:
        // TODO - area selection algo + issue commands
        break;

    case T::Suppress:
        // TODO - area selection algo + issue commands
        break;

    case T::AttackArea:
        // TODO - area selection algo + issue commands
        break;

    case T::AttackUnit:
        doCommandCreateAndIssue(squad, [&game, &cmd]() {
            return game.commandStore().unitMakeAttack(cmd.getTarget(), cmd.getAggroLevel());
        });
        break;

    default:
        break;
    }
}

void SquadManager::checkUnitCommands(Squad& squad) {
    std::size_t succeeded = 0;

    // TODO - check all terminal + determine result + mark squad command
}

} // namespace unit
} // namespace core

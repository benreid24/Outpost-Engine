#include <Core/Unit/Squad.hpp>

#include <Core/Game.hpp>

namespace core
{
namespace unit
{
Squad::Squad(fcn::FactionId faction)
: faction(faction) {}

bool Squad::addUnit(com::Unit* unit) {
    if (unit->getFaction() != faction) { return false; }
    for (auto& u : units) {
        if (u.unit == unit) { return true; }
    }

    auto& game = bl::game::Game::getInstance<Game>();
    units.emplace_back(unit, game.engine().ecs().getComponent<com::UnitAI>(unit->getId()));
    return true;
}

void Squad::removeUnit(com::Unit* unit) {
    for (auto it = units.begin(); it != units.end(); ++it) {
        if (it->unit == unit) {
            units.erase(it);
            return;
        }
    }
}

void Squad::clearUnits() { units.clear(); }

void Squad::queueCommand(const cmd::SquadCommandHandle& cmd, cmd::AddMode addMode) {
    auto copy = cmd;
    commandQueue.queueCommand(std::move(copy), addMode);
}

} // namespace unit
} // namespace core

#include <Core/Unit/Squad.hpp>

namespace core
{
namespace unit
{
Squad::Squad(fcn::FactionId faction)
: faction(faction) {}

bool Squad::addUnit(com::Unit* unit) {
    if (unit->getFaction() != faction) { return false; }
    for (com::Unit* u : units) {
        if (u == unit) { return true; }
    }
    units.emplace_back(unit);
    return true;
}

void Squad::removeUnit(com::Unit* unit) {
    for (auto it = units.begin(); it != units.end(); ++it) {
        if (*it == unit) {
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

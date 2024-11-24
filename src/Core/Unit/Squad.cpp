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

bool Squad::queueCommand(const cmd::SquadCommandHandle& cmd) {
    if (!queuedCommands.full()) {
        queuedCommands.emplace_back(makeCommandHandle(cmd));
        queuedCommands.back().markQueued();
        return true;
    }
    return false;
}

} // namespace unit
} // namespace core

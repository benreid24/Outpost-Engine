#include <Core/Player/HUD/TempSquadController.hpp>

#include <Core/Player/Player.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace player
{
namespace hud
{
TempSquadController::TempSquadController(Player& owner)
: owner(owner) {}

void TempSquadController::init() {
    // TODO
}

void TempSquadController::reset() {
    // TODO
}

bool TempSquadController::processEvent(const Event& event) {
    // TODO
    return false;
}

} // namespace hud
} // namespace player
} // namespace core

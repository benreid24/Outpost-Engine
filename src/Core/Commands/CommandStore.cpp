#include <Core/Commands/CommandStore.hpp>

namespace core
{
namespace cmd
{
CommandStore::CommandStore()
: unitCommands() {}

ExternalHandle<UnitCommand> CommandStore::unitMakeMove(const glm::vec2& position,
                                                       const world::Node* node) {
    Ref<UnitCommand> ref = unitCommands.allocate(
        UnitCommand::Type::MoveToPosition, UnitCommand::ConcurrencyType::Movement, position, node);
    return ExternalHandle<UnitCommand>(std::move(ref));
}

ExternalHandle<UnitCommand> CommandStore::unitMakeAttack(com::Unit* target) {
    Ref<UnitCommand> ref = unitCommands.allocate(
        UnitCommand::Type::KillUnit, UnitCommand::ConcurrencyType::Shooting, target);
    return ExternalHandle<UnitCommand>(std::move(ref));
}

} // namespace cmd
} // namespace core

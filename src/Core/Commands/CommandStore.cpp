#include <Core/Commands/CommandStore.hpp>

namespace core
{
namespace cmd
{
CommandStore::CommandStore()
: unitCommands() {}

UnitCommandHandle CommandStore::unitMakeMove(const glm::vec2& position, const world::Node* node) {
    Ref<UnitCommand> ref = unitCommands.allocate(
        UnitCommand::Type::MoveToPosition, UnitCommand::ConcurrencyType::Movement, position, node);
    return UnitCommandHandle(std::move(ref));
}

UnitCommandHandle CommandStore::unitMakeAttack(com::Unit* target) {
    Ref<UnitCommand> ref = unitCommands.allocate(
        UnitCommand::Type::KillUnit, UnitCommand::ConcurrencyType::Shooting, target);
    return UnitCommandHandle(std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeMove(const glm::vec2& pos) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::MoveToPosition, pos);
    return SquadCommandHandle(std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeAttack(const glm::vec2& pos) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::AttackArea, pos);
    return SquadCommandHandle(std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeAttack(com::Unit* unit) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::AttackUnit, unit);
    return SquadCommandHandle(std::move(ref));
}

} // namespace cmd
} // namespace core

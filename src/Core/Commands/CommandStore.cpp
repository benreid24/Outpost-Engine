#include <Core/Commands/CommandStore.hpp>

#include <Core/Game.hpp>

namespace core
{
namespace cmd
{
CommandStore::CommandStore()
: unitCommands() {}

UnitCommandHandle CommandStore::unitMakeMove(const glm::vec2& position, AggroLevel aggroLevel) {
    Ref<UnitCommand> ref = unitCommands.allocate(UnitCommand::Type::Move, aggroLevel, position);
    return UnitCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                             std::move(ref));
}

UnitCommandHandle CommandStore::unitMakeAttack(com::Combatant* target, AggroLevel aggroLevel) {
    Ref<UnitCommand> ref = unitCommands.allocate(UnitCommand::Type::Attack, aggroLevel, target);
    return UnitCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                             std::move(ref));
}

UnitCommandHandle CommandStore::unitMakeIdle(AggroLevel aggroLevel) {
    Ref<UnitCommand> ref = unitCommands.allocate(UnitCommand::Type::Idle, aggroLevel);
    return UnitCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                             std::move(ref));
}

UnitCommandHandle CommandStore::unitMakeSuppress(const glm::vec2& pos, AggroLevel aggroLevel) {
    Ref<UnitCommand> ref = unitCommands.allocate(UnitCommand::Type::Suppress, aggroLevel, pos);
    return UnitCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                             std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeIdle(AggroLevel aggroLevel) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::Idle, aggroLevel);
    return SquadCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                              std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeMove(const glm::vec2& pos, AggroLevel aggroLevel) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::Move, aggroLevel, pos);
    return SquadCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                              std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeAttack(const glm::vec2& pos, AggroLevel aggroLevel) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::AttackArea, aggroLevel, pos);
    return SquadCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                              std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeAttack(com::Combatant* unit, AggroLevel aggroLevel) {
    Ref<SquadCommand> ref =
        squadCommands.allocate(SquadCommand::Type::AttackUnit, aggroLevel, unit);
    return SquadCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                              std::move(ref));
}

SquadCommandHandle CommandStore::squadMakeSuppress(const glm::vec2& pos, AggroLevel aggroLevel) {
    Ref<SquadCommand> ref = squadCommands.allocate(SquadCommand::Type::Suppress, aggroLevel, pos);
    return SquadCommandHandle(bl::game::Game::getInstance<core::Game>().gameSignalChannel(),
                              std::move(ref));
}

} // namespace cmd
} // namespace core

#include <Core/Unit/Command.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace unit
{
Command::Command()
: status(Queued)
, concurrencyType(ConcurrencyType::COUNT)
, type(Type::Invalid) {}

Command::Command(const Command& copy) { *this = copy; }

Command& Command::operator=(const Command& copy) {
    status          = copy.status;
    concurrencyType = copy.concurrencyType;
    type            = copy.type;
    switch (type) {
    case Type::MoveToNode:
        targetNode = copy.targetNode;
        break;
    case Type::KillUnit:
        targetUnit = copy.targetUnit;
        break;
    }
    return *this;
}

Command Command::makeMoveToNodeCommand(world::Node* node) {
    Command cmd;
    cmd.concurrencyType = ConcurrencyType::Movement;
    cmd.type            = Type::MoveToNode;
    cmd.targetNode      = node;
    return cmd;
}

Command Command::makeKillUnitCommand(bl::ecs::Entity entity, com::Unit* unit) {
    Command cmd;
    cmd.concurrencyType   = ConcurrencyType::Shooting;
    cmd.type              = Type::KillUnit;
    cmd.targetUnit.entity = entity;
    cmd.targetUnit.unit   = unit;
    return cmd;
}

void Command::markInProgress() {
    if (status != Queued) {
        BL_LOG_WARN << "Command moved to Current from invalid status: " << status;
    }
    status = Current;
}

void Command::markComplete() {
    if (status != Current) {
        BL_LOG_WARN << "Command moved to Complete from invalid status: " << status;
    }
    status = Complete;
}

void Command::markFailed() {
    if (status != Current) {
        BL_LOG_WARN << "Command moved to Failed from invalid status: " << status;
    }
    status = Failed;
}

} // namespace unit
} // namespace core

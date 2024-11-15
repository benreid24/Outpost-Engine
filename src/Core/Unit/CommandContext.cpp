#include <Core/Unit/CommandContext.hpp>

namespace core
{
namespace unit
{
void CommandContext::init(const cmd::UnitCommand& cmd) {
    using Type = cmd::UnitCommand::Type;
    switch (cmd.getType()) {
    case Type::MoveToPosition:
        data.emplace<PathContext>();
        break;
    case Type::KillUnit:
        data.emplace<ShootContext>();
        break;
    default:
        data.emplace<std::monostate>();
        break;
    }
}

void CommandContext::clear() { data.emplace<std::monostate>(); }

CommandContext::PathContext& CommandContext::getPathContext() {
    return *std::get_if<PathContext>(&data);
}

CommandContext::ShootContext& CommandContext::getShootContext() {
    return *std::get_if<ShootContext>(&data);
}

} // namespace unit
} // namespace core

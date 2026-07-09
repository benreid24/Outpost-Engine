#include <Core/Commands/Queue.hpp>

#include <Core/Game.hpp>

namespace core
{
namespace cmd
{
namespace detail
{
bl::sig::Channel& getGameChannel() {
    return bl::game::Game::getInstance<core::Game>().gameSignalChannel();
}

} // namespace detail
} // namespace cmd
} // namespace core

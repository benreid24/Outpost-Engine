#include <Game/Game.hpp>

#include <Core/Properties.hpp>
#include <Game/ConstantsGame.hpp>
#include <Game/States/CombatDemo.hpp>
#include <iostream>

namespace game
{
namespace
{
// one instance should exist globally
Game game;
} // namespace

bool Game::performEarlyStartup(int, char**) {
    if (!performSharedEarlyStartup(ConstantsGame::ConfigFilePath)) { return false; }

#ifdef BLIB_DEBUG
    bl::logging::Config::rollLogs("logs", "verbose", 3);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Info);
    bl::logging::Config::addFileOutput("logs/verbose.log", bl::logging::Config::Trace);
#else
    bl::logging::Config::rollLogs("logs", "error", 3);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Warn);
    bl::logging::Config::addFileOutput("logs/error.log", bl::logging::Config::Warn);
#endif

    // if no previous value then use default game specific value
    if (core::Properties.WindowWidth.get() == core::Constants::UnsetUintProperty ||
        core::Properties.WindowHeight.get() == core::Constants::UnsetUintProperty) {
        core::Properties.WindowWidth.set(ConstantsGame::WindowWidth);
        core::Properties.WindowHeight.set(ConstantsGame::WindowHeight);
    }

    // TODO - SETUP_TASK - any other game specific early setup steps

    return true;
}

bl::engine::Settings Game::createStartupParameters() {
    return bl::engine::Settings().fromConfig().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(
                core::Properties.WindowWidth.get(), core::Properties.WindowHeight.get(), 32))
            .fromConfig()
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Outpost Engine")
            .withLetterBoxOnResize(true)
            .withInitialViewSize(
                sf::Vector2f(ConstantsGame::WindowWidth, ConstantsGame::WindowHeight)));
}

bool Game::completeStartup(bl::engine::Engine& engine) {
    if (!performSharedStartupCompletion(engine)) { return false; }

    // TODO - SETUP_TASK - register game specific systems

    return true;
}

bl::engine::State::Ptr Game::createInitialEngineState() {
    // TODO - SETUP_TASK - create actual initial state
    return std::make_shared<state::CombatDemo>(engine());
}

} // namespace game

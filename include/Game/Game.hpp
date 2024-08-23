#ifndef GAME_GAME_HPP
#define GAME_GAME_HPP

#include <Core/Game.hpp>

/**
 * @defgroup Game
 * @brief All classes and modules that implement the game itself
 */

/// All classes and modules that implement the game itself
namespace game
{
class Game : public core::Game {
protected:
    /**
     * @brief Called at the beginning of main(). Loads game config and properties
     *
     * @param argc The number of arguments passed to main()
     * @param argv The arguments passed to main()
     * @return True on success, false to exit
     */
    virtual bool performEarlyStartup(int argc, char** argv) override;

    /**
     * @brief Creates the parameters to launch the engine with
     */
    virtual bl::engine::Settings createStartupParameters() override;

    /**
     * @brief Creates game-specific systems, sets up the input mappings, etc
     *
     * @param engine The game engine instance
     * @return True on success, false to exit
     */
    virtual bool completeStartup(bl::engine::Engine& engine) override;

    /**
     * @brief Creates the engine state to launch the game with
     */
    virtual bl::engine::State::Ptr createInitialEngineState() override;
};

} // namespace game

#endif

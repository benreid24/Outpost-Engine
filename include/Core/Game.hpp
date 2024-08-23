#ifndef CORE_GAME_HPP
#define CORE_GAME_HPP

#include <BLIB/Game.hpp>

/**
 * @defgroup Core
 * @brief Shared library containing code used by both the game and editor
 */

/// Contains everything shared between the game and editor
namespace core
{
class Game : public bl::game::Game {
public:
    // TODO - SETUP_TASK - provide common interface here for systems etc

protected:
    /**
     * @brief Early startup tasks shared by both editor and game go in here
     *
     * @param configFilename Base filename of the config file in Properties.DataDirectory
     * @return True on success, false to exit
     */
    bool performSharedEarlyStartup(const char* configFilename);

    /**
     * @brief Final startup tasks shared by both editor and game go in here
     *
     * @param engine The game engine instance
     * @return True on success, false to exit
     */
    bool performSharedStartupCompletion(bl::engine::Engine& engine);

    /**
     * @brief Called after the main engine loop while the engine instance exists
     */
    virtual void startShutdown() override;

    /**
     * @brief Called at the end of main() after the engine instance is destroyed
     */
    virtual void completeShutdown() override;
};

} // namespace core

#endif

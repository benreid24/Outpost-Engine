#ifndef EDITOR_EDITOR_HPP
#define EDITOR_EDITOR_HPP

#include <Core/Game.hpp>

/**
 * @defgroup Editor
 * @brief All classes and modules that implement the editor for the game
 */

/// All classes and modules that implement the editor for the game
namespace editor
{
class Editor : public core::Game {
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
     * @brief Creates editor-specific systems and completes startup
     *
     * @param engine The game engine instance
     * @return True on success, false to exit
     */
    virtual bool completeStartup(bl::engine::Engine& engine) override;

    /**
     * @brief Creates the engine state to launch the editor with
     */
    virtual bl::engine::State::Ptr createInitialEngineState() override;
};

} // namespace editor

#endif

#include <Core/Game.hpp>

#include <BLIB/Events.hpp>
#include <Core/Input/Control.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace
{
struct WindowSizePersister : public bl::event::Listener<bl::engine::event::WindowResized> {
    const char* configFilename;

    virtual void observe(const bl::engine::event::WindowResized& event) override {
        core::Properties.WindowWidth.set(event.window.getSfWindow().getSize().x);
        core::Properties.WindowHeight.set(event.window.getSfWindow().getSize().y);
        core::Properties.save(configFilename);
    }
} windowSizePersister;
} // namespace

bool Game::performSharedEarlyStartup(const char* configFilename) {
    if (!Properties.load(configFilename)) { return false; }

    // TODO - SETUP_TASK - setup resource loaders for custom resource types

    // TODO - SETUP_TASK - keep or remove window size persistence
    windowSizePersister.configFilename = configFilename;

    Properties.save(configFilename);

    return true;
}

bool Game::performSharedStartupCompletion(bl::engine::Engine& engine) {
    input::configureInputSystem(engine.inputSystem());

    // TODO - SETUP_TASK - register shared engine systems here

    // TODO - SETUP_TASK - keep or remove window size persistence
    bl::event::Dispatcher::subscribe(&windowSizePersister);

    return true;
}

void Game::startShutdown() {
    // TODO - SETUP_TASK - any early shutdown tasks

    // TODO - SETUP_TASK - keep or remove window size persistence
    bl::event::Dispatcher::unsubscribe(&windowSizePersister);
}

void Game::completeShutdown() {
    // TODO - SETUP_TASK - any final shutdown tasks
}

} // namespace core

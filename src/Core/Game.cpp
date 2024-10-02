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

    windowSizePersister.configFilename = configFilename;

    Properties.save(configFilename);

    return true;
}

bool Game::performSharedStartupCompletion(bl::engine::Engine& engine) {
    input::configureInputSystem(engine.inputSystem());

    using Stage = bl::engine::FrameStage;
    using Mask  = bl::engine::StateMask::V;
    enginePtr   = &engine;
    render = &engine.systems().registerSystem<sys::Render>(Stage::RenderEarlyRefresh, Mask::All);
    damage.init(engine);
    physics = &engine.systems().getSystem<bl::sys::Physics2D>();
    entityActions =
        &engine.systems().registerSystem<sys::EntityActions>(Stage::Update1, Mask::Running);

    bl::event::Dispatcher::subscribe(&windowSizePersister);
    bl::event::Dispatcher::subscribe(&damage);

    return true;
}

void Game::startShutdown() {
    // TODO - SETUP_TASK - any early shutdown tasks

    bl::event::Dispatcher::unsubscribe(&windowSizePersister);
    bl::event::Dispatcher::unsubscribe(&damage);
}

void Game::completeShutdown() {
    // TODO - SETUP_TASK - any final shutdown tasks
}

} // namespace core

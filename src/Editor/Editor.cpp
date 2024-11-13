#include <Editor/Editor.hpp>

#include <Core/DemoEngineState.hpp>
#include <Core/Properties.hpp>
#include <Editor/ConstantsEditor.hpp>
#include <iostream>

namespace editor
{
namespace
{
// one instance should exist globally
Editor editor;
} // namespace

bool Editor::performEarlyStartup(int argc, char** argv) {
    if (!performSharedEarlyStartup(ConstantsEditor::ConfigFilePath)) { return false; }

    bl::logging::Config::rollLogs("logs", "editor", 3);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    bl::logging::Config::addFileOutput("logs/editor.log", bl::logging::Config::Debug);

    // if no previous value then use default editor specific value
    if (core::Properties.WindowWidth.get() == core::Constants::UnsetUintProperty ||
        core::Properties.WindowHeight.get() == core::Constants::UnsetUintProperty) {
        core::Properties.WindowWidth.set(ConstantsEditor::WindowWidth);
        core::Properties.WindowHeight.set(ConstantsEditor::WindowHeight);
    }

    // we will use a virtual coordinate space for now
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(ConstantsEditor::WindowWidth,
                                                      ConstantsEditor::WindowHeight);

    // TODO - SETUP_TASK - any other editor specific early setup steps

    return true;
}

bl::engine::Settings Editor::createStartupParameters() {
    return bl::engine::Settings().fromConfig().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(
                core::Properties.WindowWidth.get(), core::Properties.WindowHeight.get(), 32))
            .fromConfig()
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("BLIB Editor") // TODO - SETUP_TASK - set window title
            .withLetterBoxOnResize(false));
}

bool Editor::completeStartup(bl::engine::Engine& engine) {
    if (!performSharedStartupCompletion(engine)) { return false; }

    // TODO - SETUP_TASK - register editor specific systems

    return true;
}

bl::engine::State::Ptr Editor::createInitialEngineState() {
    // TODO - SETUP_TASK - create actual initial state
    return core::DemoEngineState::create();
}

} // namespace editor

#include <Core/Input/Control.hpp>

namespace core
{
namespace input
{
void configureInputSystem(bl::input::InputSystem& system) {
    // TODO - SETUP_TASK - configure the controls for the game

    // register controls
    system.setControlCount(Control::Count);
    system.configureTriggerControl(Control::MoveUp);
    system.configureTriggerControl(Control::MoveRight);
    system.configureTriggerControl(Control::MoveDown);
    system.configureTriggerControl(Control::MoveLeft);
    system.configureTriggerControl(Control::Sprint);
    system.configureTriggerControl(Control::Interact);
    system.configureTriggerControl(Control::Pause);
    system.configureTriggerControl(Control::Back);

    // creator the actor. Peoplemon is only 1 player locally
    bl::input::Actor& user = system.getActor();

    // set keyboard and mouse defaults. no defaults for controller
    user.getKBMTriggerControl(Control::MoveUp).triggerOnKey(sf::Keyboard::Key::W);
    user.getKBMTriggerControl(Control::MoveRight).triggerOnKey(sf::Keyboard::Key::D);
    user.getKBMTriggerControl(Control::MoveDown).triggerOnKey(sf::Keyboard::Key::S);
    user.getKBMTriggerControl(Control::MoveLeft).triggerOnKey(sf::Keyboard::Key::A);
    user.getKBMTriggerControl(Control::Sprint).triggerOnKey(sf::Keyboard::Key::LShift);
    user.getKBMTriggerControl(Control::Interact).triggerOnKey(sf::Keyboard::Key::Space);
    user.getKBMTriggerControl(Control::Pause).triggerOnKey(sf::Keyboard::Key::Enter);
    user.getKBMTriggerControl(Control::Back).triggerOnKey(sf::Keyboard::Key::LControl);

    // load user preferences from config
    system.loadFromConfig();
}

} // namespace input
} // namespace core

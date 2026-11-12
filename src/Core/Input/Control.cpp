#include <Core/Input/Control.hpp>

namespace core
{
namespace input
{
namespace
{
using Trigger = bl::input::Trigger;
using K       = sf::Keyboard::Key;
using Cfg     = bl::input::Control::Config;
using Ctrl    = bl::input::InputControlConfig;

const bl::input::InputConfiguration
    Config{.controls = {
               Ctrl{.control = Control::Movement,
                    .type    = bl::input::Control::Type::Movement,
                    .kbmConfig = Cfg{.movement = {.type =
                                                      bl::input::MovementControl::Type::Triggers,
                                                  .triggers =
                                                      {
                                                          .up = {.type = Trigger::Type::Key, .key = K::W},
                                                          .right = {.type = Trigger::Type::Key,
                                                                    .key  = K::D},
                                                          .down  = {.type = Trigger::Type::Key,
                                                                    .key  = K::S},
                                                          .left  = {.type = Trigger::Type::Key,
                                                                    .key  = K::A},
                                                      }}},
                    .joystickConfig = Cfg{.movement = {}}},

               Ctrl{.control   = Control::Raise,
                    .type      = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                .key  = K::Space}}},

               Ctrl{.control   = Control::Lower,
                    .type      = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                .key  = K::LControl}}},

               Ctrl{.control        = Control::Sprint,
                    .type           = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig      = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                     .key  = K::LShift}},
                    .joystickConfig = Cfg{.trigger = {}}},

               Ctrl{.control        = Control::Interact,
                    .type           = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig      = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                     .key  = K::Space}},
                    .joystickConfig = Cfg{.trigger = {}}},

               Ctrl{.control        = Control::Pause,
                    .type           = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig      = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                     .key  = K::Enter}},
                    .joystickConfig = Cfg{.trigger = {}}},

               Ctrl{.control        = Control::Back,
                    .type           = bl::input::Control::Type::SingleTrigger,
                    .kbmConfig      = Cfg{.trigger = Trigger::Config{.type = Trigger::Type::Key,
                                                                     .key  = K::LControl}},
                    .joystickConfig = Cfg{.trigger = {}}},

               Ctrl{.control = Control::CameraAim,
                    .type    = bl::input::Control::Type::Directional,
                    .kbmConfig =
                        Cfg{.directional = {.type = bl::input::DirectionalControl::Type::Mouse}},
                    .joystickConfig = Cfg{.trigger = {}}}}};
} // namespace

void configureInputSystem(bl::input::InputSystem& system) { system.configure(Config); }

} // namespace input
} // namespace core

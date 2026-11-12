#include <Game/States/ArenaState.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Input/InputSystem.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <Core/Cameras/ArenaController.hpp>
#include <Core/Input/Control.hpp>

#include <BLIB/Cameras/3D/Controllers/FreeController.hpp>

namespace game
{
namespace state
{
namespace controls
{

const bl::cam::c3d::FreeController::ControlScheme CameraControls{
    .movement      = core::input::Control::Movement,
    .up            = core::input::Control::Raise,
    .down          = core::input::Control::Lower,
    .speed         = core::input::Control::Sprint,
    .speedFactor   = 3.f,
    .direction     = core::input::Control::CameraAim,
    .rotationSpeed = 1.5f};

} // namespace controls

namespace
{
constexpr float TerrainWidth      = 1000.f;
constexpr float TerrainHeight     = 1000.f;
constexpr float TerrainMaxHeight  = 200.f;
constexpr float TerrainHalfWidth  = TerrainWidth * 0.5f;
constexpr float TerrainHalfHeight = TerrainHeight * 0.5f;

bool cameraActive = true;

class CustomFreeController : public bl::cam::c3d::FreeController {
public:
    CustomFreeController(const core::arena::Terrain& terrain)
    : bl::cam::c3d::FreeController(500.f, 100.f, 0.2f)
    , terrain(terrain) {
        FreeController::setControlScheme(controls::CameraControls);
    }

    virtual ~CustomFreeController() = default;

    virtual void update(float dt) override {
        if (!cameraActive) { return; }

        bl::cam::c3d::FreeController::update(dt);

        glm::vec3 pos = camera().getPosition();
        if (pos.x < -TerrainHalfWidth) { pos.x = -TerrainHalfWidth; }
        if (pos.x > TerrainHalfWidth) { pos.x = TerrainHalfWidth; }
        if (pos.z < -TerrainHalfHeight) { pos.z = -TerrainHalfHeight; }
        if (pos.z > TerrainHalfHeight) { pos.z = TerrainHalfHeight; }
        const float h = terrain.sampleHeight({pos.x, pos.z});
        if (pos.y < h + 0.5f) { pos.y = h + 0.5f; }
        camera().setPosition(pos);
    }

private:
    const core::arena::Terrain& terrain;
};

bool trainStopped = false;

} // namespace

ArenaState::ArenaState()
: State(bl::engine::StateMask::Running)
, inputSystem(nullptr) {}

const char* ArenaState::name() const { return "ArenaState"; }

void ArenaState::activate(bl::engine::Engine& engine) {
    auto world  = engine.getPlayer().enterWorld<bl::engine::World3D>();
    inputSystem = &engine.inputSystem();

    const float camHeight = TerrainMaxHeight * 0.5f;
    auto* cam             = engine.getPlayer().getRenderObserver().setCamera<bl::cam::Camera3D>(
        glm::vec3(0.f, camHeight, 0.f), 0.f, -45.f);
    /* auto* controller =
         cam->pushController<core::cam::ArenaController>(arena,
                                                        10.f,
                                                        1000.f,
                                                        15,
                                                        glm::vec2(TerrainWidth, TerrainHeight),
                                                        10.f,
                                                        0.03f,
                                                        glm::vec3(0.f, camHeight, 0.f));*/
    auto* controller = cam->pushController<CustomFreeController>(arena.getTerrain());
    controller->subscribeToInput(engine.getPlayer());

    world->typedScene().getLighting().modifySun().color.setLighting(
        bl::rc::Color(1.f, 1.f, 1.f), 1.f, 0.25f, 1.f, 0.1f);
    world->typedScene().getLighting().modifySun().dir =
        glm::normalize(glm::vec3(-2.f, -0.5f, -1.5f));

    arena.generate(0, glm::vec2(TerrainWidth, TerrainHeight), TerrainMaxHeight);
    arena.addToWorld(*world);

    grabWindow();

    subscribe(engine.getSignalChannel());
}

void ArenaState::deactivate(bl::engine::Engine& engine) {
    if (inputSystem) { inputSystem->releaseMouse(); }
    unsubscribe();
    engine.getPlayer().leaveWorld();
}

void ArenaState::update(bl::engine::Engine&, float dt, float) {
    if (!trainStopped) { arena.update(dt); }
}

void ArenaState::process(const sf::Event& event) {
    if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::G) {
            arena.generate(
                bl::rand::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max()),
                glm::vec2(TerrainWidth, TerrainHeight),
                TerrainMaxHeight);
        }
        if (key->code == sf::Keyboard::Key::Escape) {
            if (inputSystem) { inputSystem->releaseMouse(); }
            cameraActive = false;
            trainStopped = true;
        }
        if (key->code == sf::Keyboard::Key::T) { trainStopped = !trainStopped; }
    }
    else if (auto* mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) { grabWindow(); }
    }
    else if (auto* focused = event.getIf<sf::Event::FocusGained>()) { grabWindow(); }
}

void ArenaState::grabWindow() {
    if (inputSystem) { inputSystem->captureMouse(); }
    cameraActive = true;
    trainStopped = false;
}

} // namespace state
} // namespace game

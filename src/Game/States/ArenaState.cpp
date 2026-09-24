#include <Game/States/ArenaState.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <Core/Cameras/ArenaController.hpp>

#include <BLIB/Cameras/3D/Controllers/FreeController.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float TerrainWidth      = 1000.f;
constexpr float TerrainHeight     = 1000.f;
constexpr float TerrainMaxHeight  = 200.f;
constexpr float TerrainHalfWidth  = TerrainWidth * 0.5f;
constexpr float TerrainHalfHeight = TerrainHeight * 0.5f;

bool cameraActive = true;

class CustomFreeController
: public bl::cam::c3d::FreeController
, public bl::sig::Listener<sf::Event> {
public:
    CustomFreeController(sf::WindowBase& window, const core::arena::Terrain& terrain)
    : bl::cam::c3d::FreeController(500.f, 100.f, 0.2f)
    , window(window)
    , terrain(terrain) {}

    virtual ~CustomFreeController() = default;

    virtual void update(float dt) override {
        if (!cameraActive) { return; }

        float windowCenterX = static_cast<float>(window.getSize().x) / 2.f;
        float windowCenterY = static_cast<float>(window.getSize().y) / 2.f;
        const float dx      = sf::Mouse::getPosition(window).x - windowCenterX;
        const float dy      = sf::Mouse::getPosition(window).y - windowCenterY;
        applyYaw(-dx * 0.01f);
        applyPitch(dy * 0.01f);
        sf::Mouse::setPosition(sf::Vector2i(windowCenterX, windowCenterY), window);

        moveForward(forwardFactor * speedFactor);
        moveRight(rightFactor * speedFactor);
        moveUp(upFactor * speedFactor);
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

    virtual void process(const sf::Event& event) override {
        if (!cameraActive) { return; }

        if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::W) { forwardFactor = 1.f; }
            else if (key->code == sf::Keyboard::Key::S) { forwardFactor = -1.f; }
            else if (key->code == sf::Keyboard::Key::A) { rightFactor = -1.f; }
            else if (key->code == sf::Keyboard::Key::D) { rightFactor = 1.f; }
            else if (key->code == sf::Keyboard::Key::Space) { upFactor = 1.f; }
            else if (key->code == sf::Keyboard::Key::LControl) { upFactor = -1.f; }
            else if (key->code == sf::Keyboard::Key::LShift) { speedFactor = 2.f; }
        }
        else if (auto* key = event.getIf<sf::Event::KeyReleased>()) {
            if (key->code == sf::Keyboard::Key::W || key->code == sf::Keyboard::Key::S) {
                forwardFactor = 0.f;
            }
            else if (key->code == sf::Keyboard::Key::A || key->code == sf::Keyboard::Key::D) {
                rightFactor = 0.f;
            }
            else if (key->code == sf::Keyboard::Key::Space ||
                     key->code == sf::Keyboard::Key::LControl) {
                upFactor = 0.f;
            }
            else if (key->code == sf::Keyboard::Key::LShift) { speedFactor = 1.f; }
        }
    }

private:
    const core::arena::Terrain& terrain;
    sf::WindowBase& window;
    float speedFactor   = 1.f;
    float forwardFactor = 0.f;
    float rightFactor   = 0.f;
    float upFactor      = 0.f;
};
} // namespace

ArenaState::ArenaState()
: State(bl::engine::StateMask::Running) {}

const char* ArenaState::name() const { return "ArenaState"; }

void ArenaState::activate(bl::engine::Engine& engine) {
    auto world = engine.getPlayer().enterWorld<bl::engine::World3D>();

    const float camHeight = TerrainMaxHeight * 0.5f;
    auto* cam             = engine.getPlayer().getRenderObserver().setCamera<bl::cam::Camera3D>(
        glm::vec3(0.f, camHeight, 0.f), 0.f, -45.f);
    /* auto* controller =
         cam->setController<core::cam::ArenaController>(arena,
                                                        10.f,
                                                        1000.f,
                                                        15,
                                                        glm::vec2(TerrainWidth, TerrainHeight),
                                                        10.f,
                                                        0.03f,
                                                        glm::vec3(0.f, camHeight, 0.f));*/
    auto* controller = cam->setController<CustomFreeController>(
        engine.renderer().getWindow().getSfWindow(), arena.getTerrain());
    controller->subscribe(engine.getSignalChannel());

    world->typedScene().getLighting().modifySun().color.setLighting(
        bl::rc::Color(1.f, 1.f, 1.f), 1.f, 0.25f, 1.f, 0.1f);
    world->typedScene().getLighting().modifySun().dir =
        glm::normalize(glm::vec3(-2.f, 0.5f, -1.5f));

    arena.generate(0, glm::vec2(TerrainWidth, TerrainHeight), TerrainMaxHeight);
    arena.addToWorld(*world);

    window = &engine.renderer().getWindow().getSfWindow();
    grabWindow();

    subscribe(engine.getSignalChannel());
}

void ArenaState::deactivate(bl::engine::Engine& engine) {
    unsubscribe();
    engine.getPlayer().leaveWorld();
}

void ArenaState::update(bl::engine::Engine&, float, float) {}

void ArenaState::process(const sf::Event& event) {
    if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::G) {
            arena.generate(
                bl::rand::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max()),
                glm::vec2(TerrainWidth, TerrainHeight),
                TerrainMaxHeight);
        }
        if (key->code == sf::Keyboard::Key::Escape) {
            window->setMouseCursorGrabbed(false);
            window->setMouseCursorVisible(true);
            cameraActive = false;
        }
    }
    else if (auto* mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) { grabWindow(); }
    }
    else if (auto* focused = event.getIf<sf::Event::FocusGained>()) { grabWindow(); }
}

void ArenaState::grabWindow() {
    window->setMouseCursorVisible(false);
    window->setMouseCursorGrabbed(true);
    cameraActive = true;
}

} // namespace state
} // namespace game

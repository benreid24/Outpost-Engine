#include <Game/States/ArenaState.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <Core/Cameras/ArenaController.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float TerrainWidth     = 1000.f;
constexpr float TerrainHeight    = 1000.f;
constexpr float TerrainMaxHeight = 200.f;
} // namespace

ArenaState::ArenaState()
: State(bl::engine::StateMask::Running) {}

const char* ArenaState::name() const { return "ArenaState"; }

void ArenaState::activate(bl::engine::Engine& engine) {
    auto world = engine.getPlayer().enterWorld<bl::engine::World3D>();

    const float camHeight = TerrainMaxHeight * 1.5f;
    auto* cam             = engine.getPlayer().getRenderObserver().setCamera<bl::cam::Camera3D>(
        glm::vec3(0.f, camHeight, 0.f), 0.f, -45.f);
    auto* controller =
        cam->setController<core::cam::ArenaController>(10.f,
                                                       1000.f,
                                                       5,
                                                       glm::vec2(TerrainWidth, TerrainHeight),
                                                       100.f,
                                                       0.05f,
                                                       glm::vec3(0.f, camHeight, 0.f));
    controller->subscribe(engine.getSignalChannel());

    world->typedScene().getLighting().modifySun().color.setLighting(
        bl::rc::Color(1.f, 1.f, 1.f), 1.f, 0.25f, 1.f, 0.1f);
    world->typedScene().getLighting().modifySun().dir =
        glm::normalize(glm::vec3(-2.f, 0.5f, -1.5f));

    arena.generate(0, glm::vec2(TerrainWidth, TerrainHeight), TerrainMaxHeight);
    arena.addToWorld(*world);

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
                bl::util::Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max()),
                glm::vec2(TerrainWidth, TerrainHeight),
                TerrainMaxHeight);
        }
    }
}

} // namespace state
} // namespace game

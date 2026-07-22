#include <Game/States/Arena.hpp>

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
constexpr float TerrainWidth          = 1000.f;
constexpr float TerrainHeight         = 1000.f;
constexpr float TerrainMaxHeight      = 50.f;
constexpr float TerrainStep           = 30.f;
constexpr unsigned int TerrainOctaves = 1;
constexpr float TerrainPersistence    = 0.5f;
} // namespace

Arena::Arena()
: State(bl::engine::StateMask::Running) {}

const char* Arena::name() const { return "Arena"; }

void Arena::activate(bl::engine::Engine& engine) {
    auto world = engine.getPlayer().enterWorld<bl::engine::World3D>();

    const float camHeight = TerrainMaxHeight * 1.5f;
    auto* cam             = engine.getPlayer().getRenderObserver().setCamera<bl::cam::Camera3D>(
        glm::vec3(0.f, camHeight, 0.f), 0.f, -45.f);
    auto* controller = cam->setController<core::cam::ArenaController>(camHeight, 100.f, 2.f);
    controller->subscribe(engine.getSignalChannel());

    world->typedScene().getLighting().modifySun().color.setLighting(
        bl::rc::Color(1.f, 1.f, 1.f), 1.f, 0.25f, 1.f, 0.1f);
    world->typedScene().getLighting().modifySun().dir =
        glm::normalize(glm::vec3(-2.f, 0.5f, -1.5f));

    terrain.createFromNoise2d(*world,
                              perlin,
                              TerrainWidth,
                              TerrainHeight,
                              TerrainMaxHeight,
                              TerrainStep,
                              TerrainOctaves,
                              TerrainPersistence);
    colorTerrain();
    terrain.addToScene(world->scene(), bl::rc::UpdateSpeed::Static);

    subscribe(engine.getSignalChannel());
}

void Arena::deactivate(bl::engine::Engine& engine) {
    unsubscribe();
    engine.getPlayer().leaveWorld();
}

void Arena::update(bl::engine::Engine&, float, float) {}

void Arena::process(const sf::Event& event) {
    if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::G) {
            terrain.regenerateFromNoise2d(perlin,
                                          TerrainWidth,
                                          TerrainHeight,
                                          TerrainMaxHeight,
                                          TerrainStep,
                                          TerrainOctaves,
                                          TerrainPersistence);
            colorTerrain();
        }
    }
}

void Arena::colorTerrain() {
    auto& verts = terrain.component().gpuBuffer.vertices();
    for (auto& v : verts) {
        const float h = v.pos.y / TerrainMaxHeight;
        v.color       = bl::rc::Color(h, h, h);
    }
}

} // namespace state
} // namespace game

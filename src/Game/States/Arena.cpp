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
constexpr float TerrainMaxHeight      = 200.f;
constexpr float TerrainStep           = 1.f;
constexpr unsigned int TerrainOctaves = 16;
constexpr float TerrainPersistence    = 0.4f;
constexpr float TerrainFrequency      = 0.005f;

struct TerrainColor {
    float minHeight;
    float maxHeight;
    bl::rc::Color color;
};

const std::array<TerrainColor, 6> TerrainColors = {{
    {0.f, 0.43f, bl::rc::Color(0.f, 0.f, 0.6f)},          // deep water
    {0.43f, 0.45f, bl::rc::Color(0.f, 0.1f, 0.95f)},      // water
    {0.45f, 0.47f, bl::rc::Color(0.965f, 0.828f, 0.45f)}, // sand
    {0.47f, 0.55f, bl::rc::Color(0.2f, 0.8f, 0.2f)},      // grass
    {0.55f, 0.65f, bl::rc::Color(0.31f, 0.31f, 0.26f)},   // rock
    {0.65f, 1.1f, bl::rc::Color(1.5f, 1.5f, 1.5f)}        // snow
}};
constexpr float WaterHeight                     = 0.45f * TerrainMaxHeight;
} // namespace

Arena::Arena()
: State(bl::engine::StateMask::Running) {}

const char* Arena::name() const { return "Arena"; }

void Arena::activate(bl::engine::Engine& engine) {
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

    terrain.createFromNoise2d(*world,
                              perlin,
                              TerrainWidth,
                              TerrainHeight,
                              TerrainMaxHeight,
                              TerrainStep,
                              TerrainOctaves,
                              TerrainPersistence,
                              TerrainFrequency);
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
            perlin.reseed(std::chrono::system_clock::now().time_since_epoch().count());
            terrain.regenerateFromNoise2d(perlin,
                                          TerrainWidth,
                                          TerrainHeight,
                                          TerrainMaxHeight,
                                          TerrainStep,
                                          TerrainOctaves,
                                          TerrainPersistence,
                                          TerrainFrequency);
            colorTerrain();
        }
    }
}

void Arena::colorTerrain() {
    auto& verts = terrain.component().gpuBuffer.vertices();
    for (auto& v : verts) {
        const float normalizedHeight = v.pos.y / TerrainMaxHeight;
        for (const auto& tc : TerrainColors) {
            if (normalizedHeight >= tc.minHeight && normalizedHeight < tc.maxHeight) {
                v.color = tc.color.toVec4();
                break;
            }
        }
        if (v.pos.y < WaterHeight) { v.pos.y = WaterHeight; }
    }
    terrain.commitUpdate();
}

} // namespace state
} // namespace game

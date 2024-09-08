#ifndef CORE_DEMOENGINESTATE_HPP
#define CORE_DEMOENGINESTATE_HPP

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Graphics.hpp>

namespace core
{
/**
 * @brief Basic engine state that provides a spinning triangle
 */
class DemoEngineState : public bl::engine::State {
public:
    static constexpr float DegPerSec = 90.f;

    static bl::engine::State::Ptr create() { return Ptr(new DemoEngineState()); }

    virtual const char* name() const { return "DemoEngineState"; }

    virtual void activate(bl::engine::Engine& engine) {
        auto& world =
            engine.getPlayer().enterWorld<bl::engine::BasicWorld<bl::rc::scene::Scene2D>>();
        auto scene = world.scene();
        engine.renderer().getObserver().setCamera<bl::cam::Camera2D>(glm::vec2(400, 300),
                                                                     glm::vec2(800, 600));
        engine.renderer().getObserver().setClearColor(sf::Color::Cyan);

        triangle.create(world, {0.f, 173.f}, {200.f, 173.f}, {100.f, 0.f});
        triangle.setColorGradient({1.f, 0.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 1.f, 1.f});
        triangle.getTransform().setPosition(400.f, 300.f);
        triangle.getTransform().setOrigin({100.f, 123.f});
        triangle.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

        BL_LOG_INFO << "DemoEngineState activated";
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float dt, float) override {
        triangle.getTransform().rotate(dt * DegPerSec);
    }

private:
    bl::gfx::Triangle triangle;

    DemoEngineState()
    : State(bl::engine::StateMask::Running) {}
};
} // namespace core

#endif

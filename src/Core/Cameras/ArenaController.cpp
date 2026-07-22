#include <Core/Cameras/ArenaController.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>

namespace core
{
namespace cam
{
namespace
{
constexpr float HeightFactorSpeed = 2.f; // 0.5s to adjust
}
ArenaController::ArenaController(float initialHeight, float speedFactor, float dampening)
: nominalHeight(initialHeight)
, speedFactor(speedFactor)
, velocity(0.f, 0.f)
, targetVelocity(0.f, 0.f)
, dampening(dampening)
, heightFactor(1.f)
, targetHeightFactor(1.f) {}

void ArenaController::update(float dt) {
    // update height factor
    if (std::abs(targetHeightFactor - heightFactor) < 0.01f) { heightFactor = targetHeightFactor; }
    else { heightFactor += (targetHeightFactor - heightFactor) * HeightFactorSpeed * dt; }

    // update position
    glm::vec3 pos = camera().getPosition();
    pos.x += velocity.x * speedFactor * dt;
    pos.z += velocity.y * speedFactor * dt;
    pos.y = nominalHeight * heightFactor;
    camera().setPosition(pos);

    // update velocity
    velocity += (targetVelocity - velocity) * dampening * dt;
    if (std::abs(velocity.x) < 0.01f) { velocity.x = 0.f; }
    if (std::abs(velocity.y) < 0.01f) { velocity.y = 0.f; }
}

void ArenaController::process(const sf::Event& event) {
    if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->code) {
        case sf::Keyboard::Key::W:
            targetVelocity.y = -1.f;
            break;
        case sf::Keyboard::Key::S:
            targetVelocity.y = 1.f;
            break;
        case sf::Keyboard::Key::A:
            targetVelocity.x = -1.f;
            break;
        case sf::Keyboard::Key::D:
            targetVelocity.x = 1.f;
            break;
        case sf::Keyboard::Key::Q:
            targetHeightFactor *= 0.5f;
            break;
        case sf::Keyboard::Key::E:
            targetHeightFactor *= 2.f;
            break;
        default:
            break;
        }
    }
    else if (auto* key = event.getIf<sf::Event::KeyReleased>()) {
        switch (key->code) {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::S:
            targetVelocity.y = 0.f;
            break;
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::D:
            targetVelocity.x = 0.f;
            break;
        default:
            break;
        }
    }
}

} // namespace cam
} // namespace core

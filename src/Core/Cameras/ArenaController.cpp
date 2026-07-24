#include <Core/Cameras/ArenaController.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>

namespace core
{
namespace cam
{
namespace
{
constexpr float StepChangeSpeed = 2.f; // 0.5s to adjust
constexpr float MinPitch        = 15.f;
constexpr float MaxPitch        = 90.f;
constexpr float PitchIncrement  = 15.f;
constexpr float YawIncrement    = 45.f;

void updateValue(float& current, float target, float dt) {
    if (std::abs(current - target) < 0.01f) { current = target; }
    else { current += (target - current) * StepChangeSpeed * dt; }
}

void updateVelocity(float& current, float target, float dampening, float dt) {
    if (std::abs(target) > std::abs(current)) { current = target; }
    else { current *= std::pow(dampening, dt); }
}
} // namespace

ArenaController::ArenaController(float minDistance, float maxDistance, unsigned int distanceSteps,
                                 glm::vec2 arenaSize, float speed, float dampening,
                                 glm::vec3 initialPosition)
: minDistance(minDistance)
, distancePerStep((maxDistance - minDistance) / static_cast<float>(distanceSteps))
, distanceSteps(distanceSteps)
, currentDistanceStep(distanceSteps / 2)
, targetDistance(minDistance + distancePerStep * currentDistanceStep)
, currentDistance(targetDistance)
, arenaSize(arenaSize)
, currentPosition(initialPosition)
, velocity(0.f, 0.f, 0.f)
, targetVelocity(0.f, 0.f, 0.f)
, speedPerStep((speed / targetDistance * 2.f) * distancePerStep)
, speed(speed)
, speedMultiple(1.f)
, dampening(dampening)
, currentYaw(0.f)
, targetYaw(0.f)
, currentPitch(45.f)
, targetPitch(45.f) {}

void ArenaController::update(float dt) {
    // adjust scalars
    updateValue(currentDistance, targetDistance, dt);
    updateValue(currentPitch, targetPitch, dt);

    // update yaw via shortest path
    const float yawDiff = targetYaw - currentYaw;
    if (yawDiff > 180.f) { currentYaw += 360.f; }
    else if (yawDiff < -180.f) { currentYaw -= 360.f; }
    updateValue(currentYaw, targetYaw, dt);

    // adjust velocity for yaw so we move in camera space
    const float yawCos   = std::cos(glm::radians(currentYaw));
    const float yawSin   = std::sin(glm::radians(currentYaw));
    const float pitchCos = std::cos(glm::radians(currentPitch));
    const float pitchSin = std::sin(glm::radians(currentPitch));

    // update focus position
    const glm::vec3 yawAdjustedVelocity(velocity.x * yawCos + velocity.z * yawSin,
                                        velocity.y,
                                        velocity.z * yawCos - velocity.x * yawSin);
    speed = speedPerStep * static_cast<float>(currentDistanceStep + 1);
    currentPosition += yawAdjustedVelocity * speed * speedMultiple * dt;
    currentPosition.y = std::max(currentPosition.y, 0.f); // TODO - get from heightmap

    // update camera position and orientation
    glm::vec3 camPos;
    camPos.x = currentPosition.x + yawSin * pitchCos * currentDistance;
    camPos.z = currentPosition.z + yawCos * pitchCos * currentDistance;
    camPos.y = currentPosition.y + pitchSin * currentDistance;
    camPos.y = std::max(camPos.y, 1.f);
    camera().setPosition(camPos);
    camera().getOrientationForChange().lookAt(currentPosition, camPos);

    // update velocity
    updateVelocity(velocity.x, targetVelocity.x, dampening, dt);
    updateVelocity(velocity.y, targetVelocity.y, dampening, dt);
    updateVelocity(velocity.z, targetVelocity.z, dampening, dt);
}

void ArenaController::process(const sf::Event& event) {
    if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->code) {
        case sf::Keyboard::Key::W:
            targetVelocity.z = -1.f;
            break;
        case sf::Keyboard::Key::S:
            targetVelocity.z = 1.f;
            break;
        case sf::Keyboard::Key::A:
            targetVelocity.x = -1.f;
            break;
        case sf::Keyboard::Key::D:
            targetVelocity.x = 1.f;
            break;
        case sf::Keyboard::Key::R:
            targetVelocity.y = 1.f;
            break;
        case sf::Keyboard::Key::T:
            targetVelocity.y = -1.f;
            break;
        case sf::Keyboard::Key::LShift:
            speedMultiple = 2.f;
            break;

        case sf::Keyboard::Key::Hyphen:
            if (currentDistanceStep > 0) {
                --currentDistanceStep;
                targetDistance = minDistance + distancePerStep * currentDistanceStep;
            }
            break;
        case sf::Keyboard::Key::Equal:
            if (currentDistanceStep < distanceSteps) {
                ++currentDistanceStep;
                targetDistance = minDistance + distancePerStep * currentDistanceStep;
            }
            break;

        case sf::Keyboard::Key::Q:
            targetYaw -= YawIncrement;
            if (targetYaw < 0.f) { targetYaw += 360.f; }
            break;
        case sf::Keyboard::Key::E:
            targetYaw += YawIncrement;
            if (targetYaw >= 360.f) { targetYaw -= 360.f; }
            break;

        case sf::Keyboard::Key::Z:
            if (targetPitch > MinPitch) { targetPitch -= PitchIncrement; }
            break;
        case sf::Keyboard::Key::X:
            if (targetPitch < MaxPitch) { targetPitch += PitchIncrement; }
            break;

        default:
            break;
        }
    }
    else if (auto* key = event.getIf<sf::Event::KeyReleased>()) {
        switch (key->code) {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::S:
            targetVelocity.z = 0.f;
            break;
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::D:
            targetVelocity.x = 0.f;
            break;
        case sf::Keyboard::Key::R:
        case sf::Keyboard::Key::T:
            targetVelocity.y = 0.f;
            break;
        case sf::Keyboard::Key::LShift:
            speedMultiple = 1.f;
            break;
        default:
            break;
        }
    }
    else if (auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        // TODO - debounce?
        if (wheel->delta > 0.f && currentDistanceStep > 0) {
            --currentDistanceStep;
            targetDistance = minDistance + distancePerStep * currentDistanceStep;
        }
        else if (wheel->delta < 0.f && currentDistanceStep < distanceSteps) {
            ++currentDistanceStep;
            targetDistance = minDistance + distancePerStep * currentDistanceStep;
        }
    }
}

} // namespace cam
} // namespace core

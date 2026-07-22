#ifndef CORE_CAMERAS_ARENACONTROLLER_HPP
#define CORE_CAMERAS_ARENACONTROLLER_HPP

#include <BLIB/Cameras/3D/CameraController3D.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <SFML/Window/Event.hpp>

/**
 * @addtogroup Cameras
 * @ingroup Core
 * @brief Collection of cameras for the game and editor
 */

namespace core
{
/// Collection of cameras for the game
namespace cam
{
/**
 * @brief Camera controller for the arena camera
 *
 * @ingroup Cameras
 */
class ArenaController
: public bl::cam::CameraController3D
, public bl::sig::Listener<sf::Event> {
public:
    /**
     * @brief Creates a new arena camera controller
     *
     * @param minDistance The minimum distance the camera can be from the target
     * @param maxDistance The maximum distance the camera can be from the target
     * @param distanceSteps The number of steps between the min and max distance
     * @param arenaSize The size of the arena in world coordinates centered on the origin
     * @param speed The speed for camera movement in world units per second
     * @param dampening The dampening factor for camera movement
     * @param initialPosition The initial position of the camera in world coordinates
     */
    ArenaController(float minDistance, float maxDistance, unsigned int distanceSteps,
                    glm::vec2 arenaSize, float speed, float dampening,
                    glm::vec3 initialPosition = glm::vec3(0.f, 0.f, 0.f));

    /**
     * @brief Destroys the controller
     */
    virtual ~ArenaController() = default;

private:
    const float minDistance;
    const float distancePerStep;
    const unsigned int distanceSteps;
    unsigned int currentDistanceStep;
    float targetDistance;
    float currentDistance;

    const glm::vec2 arenaSize;
    glm::vec3 currentPosition;
    glm::vec3 velocity;
    glm::vec3 targetVelocity;
    const float speedPerStep;
    float speed;
    const float dampening;

    float currentYaw;
    float targetYaw;
    float currentPitch;
    float targetPitch;

    virtual void update(float dt) override;
    virtual void process(const sf::Event& event) override;
};

} // namespace cam
} // namespace core

#endif

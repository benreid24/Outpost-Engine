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
     * @param initialHeight The initial height of the camera
     * @param speedFactor The speed factor for camera movement
     * @param dampening The dampening factor for camera movement
     */
    ArenaController(float initialHeight, float speedFactor, float dampening = 0.5f);

    /**
     * @brief Destroys the controller
     */
    virtual ~ArenaController() = default;

private:
    float nominalHeight;
    float speedFactor;
    glm::vec2 velocity;
    glm::vec2 targetVelocity;
    float dampening;
    float heightFactor;
    float targetHeightFactor;

    virtual void update(float dt) override;
    virtual void process(const sf::Event& event) override;
};

} // namespace cam
} // namespace core

#endif

#ifndef CORE_SYSTEMS_MOVEMENT_HPP
#define CORE_SYSTEMS_MOVEMENT_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Systems/Physics2D.hpp>
#include <Core/World/World.hpp>

namespace core
{
namespace sys
{
/**
 * @brief System responsible for managing entity movement and state in the game world
 *
 * @ingroup Systems
 */
class Movement
: public bl::engine::System
, public bl::event::Listener<bl::sys::Physics2D::SensorEntered, bl::sys::Physics2D::SensorExited> {
public:
    /**
     * @brief Destroys the system
     */
    virtual ~Movement();

private:
    bl::engine::Engine* engine;

    virtual void init(bl::engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
    virtual void observe(const bl::sys::Physics2D::SensorEntered& event) override;
    virtual void observe(const bl::sys::Physics2D::SensorExited& event) override;
};

} // namespace sys
} // namespace core

#endif

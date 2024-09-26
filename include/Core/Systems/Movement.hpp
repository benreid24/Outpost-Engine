#ifndef CORE_SYSTEMS_MOVEMENT_HPP
#define CORE_SYSTEMS_MOVEMENT_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Engine/System.hpp>
#include <Core/Components/Moveable.hpp>

namespace core
{
namespace sys
{
/**
 * @brief Movement system. Processes entity move commands
 *
 * @ingroup Systems
 */
class Movement : public bl::engine::System {
public:
    /**
     * @brief Creates the system
     */
    Movement();

    /**
     * @brief Destroys the system
     */
    virtual ~Movement() = default;

private:
    bl::ecs::ComponentPool<com::Moveable>* pool;

    void init(bl::engine::Engine& engine) override;
    void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace sys
} // namespace core

#endif

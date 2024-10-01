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
 * @brief Entity actions system. Processes entity control commands
 *
 * @ingroup Systems
 */
class EntityActions : public bl::engine::System {
public:
    /**
     * @brief Creates the system
     */
    EntityActions();

    /**
     * @brief Destroys the system
     */
    virtual ~EntityActions() = default;

private:
    bl::ecs::ComponentPool<com::Moveable>* pool;

    void init(bl::engine::Engine& engine) override;
    void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace sys
} // namespace core

#endif

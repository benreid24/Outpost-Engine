#ifndef CORE_SYSTEMS_UNIT_HPP
#define CORE_SYSTEMS_UNIT_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <Core/Components/Unit.hpp>

namespace core
{
namespace sys
{
/**
 * @brief System that handles unit level AI and resolves unit actions
 *
 * @ingroup Systems
 * @ingroup Unit
 */
class Unit : public bl::engine::System {
public:
    /**
     * @brief Destroys the system
     */
    virtual ~Unit() = default;

private:
    bl::engine::Engine* engine;
    bl::ecs::ComponentPool<com::Unit>* units;

    void init(bl::engine::Engine& engine) override;
    void update(std::mutex&, float dt, float, float, float) override;
    void doCapabilities(float dt);
};

} // namespace sys
} // namespace core

#endif

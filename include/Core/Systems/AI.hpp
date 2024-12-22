#ifndef CORE_SYSTEMS_AI_HPP
#define CORE_SYSTEMS_AI_HPP

#include <BLIB/Engine/System.hpp>
#include <Core/Components/UnitAI.hpp>

namespace core
{
namespace sys
{
/**
 * @brief System that performs AI for units, squads, and actors
 *
 * @ingroup Systems
 */
class AI : public bl::engine::System {
public:
    /**
     * @brief Destroys the system
     */
    virtual ~AI() = default;

private:
    bl::engine::Engine* engine;
    bl::ecs::ComponentPool<com::UnitAI>* unitAIs;

    void init(bl::engine::Engine& engine) override;
    void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace sys
} // namespace core

#endif

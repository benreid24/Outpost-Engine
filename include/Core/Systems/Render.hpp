#ifndef CORE_SYSTEMS_RENDER_HPP
#define CORE_SYSTEMS_RENDER_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Color.hpp>
#include <Core/World/Node.hpp>
#include <glm/glm.hpp>

/**
 * @addtogroup Systems
 * @ingroup Core
 * @brief The core game systems
 */

namespace core
{
/// The core game systems
namespace sys
{
/**
 * @brief Core game system that manages how entities are rendered
 *
 * @ingroup Systems
 */
class Render : public bl::engine::System {
public:
    static constexpr std::uint32_t NodeEdgesPipelineId = 1000;

    /**
     * @brief Creates the system
     */
    Render();

    /**
     * @brief Destroys the system
     */
    virtual ~Render() = default;

    /**
     * @brief Adds basic testing graphics to the given entity
     *
     * @param entity The entity to add the graphical components to
     * @param size The size of the entity to render
     * @param color The color to make the test graphics
     */
    void addTestGraphicsToEntity(bl::ecs::Entity entity, glm::vec2 size, bl::rc::Color color);

    /**
     * @brief Adds basic testing graphics to the given entity
     *
     * @param entity The entity to add the graphical components to
     * @param size The size of the entity to render
     * @param color The color to make the test graphics
     */
    void addTestGraphicsToEntity(bl::ecs::Entity entity, float radius, bl::rc::Color color);

private:
    bl::engine::Engine* engine;

    void init(bl::engine::Engine& engine) override;
    void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                float realResidual) override;
};

} // namespace sys
} // namespace core

#endif

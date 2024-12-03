#ifndef CORE_UNIT_SQUADMANAGER_HPP
#define CORE_UNIT_SQUADMANAGER_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Util/RefPool.hpp>
#include <Core/Unit/Squad.hpp>

namespace core
{
namespace unit
{
/**
 * @brief Class that owns and manages squads and lifetimes and performs squad AI
 *
 * @ingroup Unit
 */
class SquadManager : public bl::engine::System {
public:
    /// Handle to a squad. Owning a handle keeps the squad in scope
    using Ref = bl::util::Ref<Squad>;

    /**
     * @brief Creates the squad manager
     */
    SquadManager();

    /**
     * @brief Creates a new squad for the given faction
     *
     * @param faction The faction to create the squad for
     * @return A handle to the new squad
     */
    Ref makeSquad(fcn::FactionId faction);

    /**
     * @brief Returns all the squads owned by the given faction
     *
     * @param faction The faction to get squads for
     * @return The list of squads owned by the faction
     */
    std::vector<Ref> getFactionSquads(fcn::FactionId faction);

    /**
     * @brief Returns the list of all current squads
     */
    const std::vector<Ref>& getAllSquads() const { return internalRefs; }

private:
    bl::util::RefPool<Squad> squads;
    std::vector<Ref> internalRefs;

    virtual void init(bl::engine::Engine& engine) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace unit
} // namespace core

#endif

#ifndef CORE_FACTIONS_FACTIONS_HPP
#define CORE_FACTIONS_FACTIONS_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <Core/Components/Unit.hpp>
#include <Core/Factions/Faction.hpp>

namespace core
{
namespace fcn
{
/**
 * @brief Faction manager that owns factions and is responsible for keeping them updated
 *
 * @ingroup Factions
 */
class Factions
: public bl::event::Listener<bl::ecs::event::ComponentAdded<com::Unit>,
                             bl::ecs::event::ComponentRemoved<com::Unit>> {
public:
    /**
     * @brief Creates the faction manager
     */
    Factions();

    /**
     * @brief Destroys the faction manager
     */
    ~Factions();

    /**
     * @brief Creates a new faction
     *
     * @param name The name of the faction
     * @return The newly created faction
     */
    Faction* createFaction(std::string_view name = "");

    /**
     * @brief Destroys the faction with the given id
     *
     * @param id The id of the faction to destroy
     */
    void destroyFaction(FactionId id);

    /**
     * @brief Tests whether the given faction exists
     *
     * @param id The id of the faction to check
     * @return True if the faction exists, false otherwise
     */
    bool factionExists(FactionId id) const { return freeIds.isAllocated(id.getIndex()); }

    /**
     * @brief Tests whether the given faction exists
     *
     * @param faction The faction to check
     * @return True if the faction exists, false otherwise
     */
    bool factionExists(const Faction* faction) const {
        return freeIds.isAllocated(faction - &factions.front().get());
    }

private:
    std::vector<bl::ctr::ObjectWrapper<Faction>> factions;
    bl::util::IdAllocator<FactionId::IdType> freeIds;

    virtual void observe(const bl::ecs::event::ComponentAdded<com::Unit>& event) override;
    virtual void observe(const bl::ecs::event::ComponentRemoved<com::Unit>& event) override;
};

} // namespace fcn
} // namespace core

#endif

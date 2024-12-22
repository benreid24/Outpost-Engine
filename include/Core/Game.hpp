#ifndef CORE_GAME_HPP
#define CORE_GAME_HPP

#include <BLIB/Game.hpp>

#include <BLIB/Systems/Physics2D.hpp>
#include <Core/Commands/CommandStore.hpp>
#include <Core/Factions/Factions.hpp>
#include <Core/Systems/AI.hpp>
#include <Core/Systems/Damage.hpp>
#include <Core/Systems/Movement.hpp>
#include <Core/Systems/Render.hpp>
#include <Core/Systems/Unit.hpp>
#include <Core/Unit/SquadManager.hpp>

/**
 * @defgroup Core
 * @brief Shared library containing code used by both the game and editor
 */

/// Contains everything shared between the game and editor
namespace core
{
class Game : public bl::game::Game {
public:
    /**
     * @brief Returns the game engine instance
     */
    bl::engine::Engine& engine() { return *enginePtr; }

    /**
     * @brief Returns the game render system
     */
    sys::Render& renderSystem() { return *render; }

    /**
     * @brief Returns the damage system
     */
    sys::Damage& damageSystem() { return damage; }

    /**
     * @brief Returns the physics system
     */
    bl::sys::Physics2D& physicsSystem() { return *physics; }

    /**
     * @brief Returns the entity actions system
     */
    sys::Unit& unitSystem() { return *units; }

    /**
     * @brief Returns the movement system
     */
    sys::Movement& movementSystem() { return *movement; }

    /**
     * @brief Returns the default font
     */
    sf::VulkanFont& defaultFont() { return font; }

    /**
     * @brief Returns the command store
     */
    cmd::CommandStore& commandStore() { return commands; }

    /**
     * @brief Returns the factions store
     */
    fcn::Factions& factions() { return factionStore; }

    /**
     * @brief Returns the squad manager
     */
    unit::SquadManager& squadManager() { return *squads; }

    /**
     * @brief Returns the AI system
     */
    sys::AI& aiSystem() { return *ai; }

protected:
    /**
     * @brief Early startup tasks shared by both editor and game go in here
     *
     * @param configFilename Base filename of the config file in Properties.DataDirectory
     * @return True on success, false to exit
     */
    bool performSharedEarlyStartup(const char* configFilename);

    /**
     * @brief Final startup tasks shared by both editor and game go in here
     *
     * @param engine The game engine instance
     * @return True on success, false to exit
     */
    bool performSharedStartupCompletion(bl::engine::Engine& engine);

    /**
     * @brief Called after the main engine loop while the engine instance exists
     */
    virtual void startShutdown() override;

    /**
     * @brief Called at the end of main() after the engine instance is destroyed
     */
    virtual void completeShutdown() override;

private:
    bl::engine::Engine* enginePtr;
    sys::Damage damage;
    sys::Render* render;
    bl::sys::Physics2D* physics;
    sys::Unit* units;
    sys::Movement* movement;
    sf::VulkanFont font;
    cmd::CommandStore commands;
    fcn::Factions factionStore;
    unit::SquadManager* squads;
    sys::AI* ai;
};

} // namespace core

#endif

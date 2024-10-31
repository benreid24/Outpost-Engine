#ifndef CORE_PLAYER_PLAYER_HPP
#define CORE_PLAYER_PLAYER_HPP

#include <BLIB/Engine/Player.hpp>
#include <Core/Player/HUD.hpp>

/**
 * @addtogroup Player
 * @ingroup Core
 * @brief Collection of classes representing the player
 */

namespace core
{
/// Collection of classes representing the player
namespace player
{
/**
 * @brief Player class that owns player specific state and UI
 *
 * @ingroup Player
 */
class Player : public bl::engine::Player {
public:
    /**
     * @brief Creates the player
     *
     * @param engine The game engine instance
     * @param observer The renderer observer to use
     * @param actor The input actor to use
     */
    Player(bl::engine::Engine& engine, bl::rc::Observer* observer, bl::input::Actor* actor);

    /**
     * @brief Destroys the player
     */
    virtual ~Player() = default;

private:
    HUD hud;

    virtual void onWorldChange(bl::engine::World* prev, bl::engine::World* next) override;
};
} // namespace player
} // namespace core

#endif

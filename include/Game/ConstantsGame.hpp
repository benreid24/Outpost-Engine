#ifndef GAME_CONSTANTSGAME_HPP
#define GAME_CONSTANTSGAME_HPP

namespace game
{
/**
 * @brief Collection of constants specific in nature or value to the game
 *
 * @ingroup Game
 */
struct ConstantsGame {
    static constexpr const char* ConfigFilePath = "config.cfg";
    static constexpr unsigned int WindowWidth   = 800;
    static constexpr unsigned int WindowHeight  = 600;
};

} // namespace game

#endif

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace core
{
/**
 * @brief Collection of constants shared across the game and editor
 *
 * @ingroup Core
 */
struct Constants {
    static constexpr unsigned int UnsetUintProperty = 0;

    static constexpr const char* AppName                = "ProjectName";
    static constexpr const char* DefaultSpritesheetPath = "Resources/Images/Spritesheets";
    static constexpr const char* DefaultMusicPath       = "Resources/Audio/Music";
};
} // namespace core

#endif

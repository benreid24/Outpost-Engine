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

    static constexpr const char* AppName                = "OutpostEngine";
    static constexpr const char* DefaultSpritesheetPath = "Resources/Images/Spritesheets";
    static constexpr const char* DefaultMusicPath       = "Resources/Audio/Music";

    static constexpr unsigned int CommandPoolInitialSize = 64;

    static constexpr float UnitAiStopDistanceDefault = 5.f;
    static constexpr float UnitDampingFactorDefault  = 0.1f;
};
} // namespace core

#endif

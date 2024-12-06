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

    static constexpr float UnitAiStopDistanceDefault          = 5.f;
    static constexpr float UnitAiFiringMaxDistanceDefault     = 450.f;
    static constexpr float UnitAiFiringLineRecheckTimeDefault = 3.f;

    static constexpr float UnitAccelerationDefault     = 320.f;
    static constexpr float UnitMaxSpeedDefault         = 1920.f / 6.f;
    static constexpr float UnitDirCorrectFactorDefault = 0.9f;
    static constexpr float UnitDampingFactorDefault    = 0.1f;
    static constexpr float UnitRotateRateDefault       = 270.f;

    static constexpr float UnitFireRateDefault     = 3.f;
    static constexpr float UnitBulletDamageDefault = 20.f;
    static constexpr float UnitBulletOffsetDefault = 60.f;
};
} // namespace core

#endif

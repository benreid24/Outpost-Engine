#ifndef CORE_PROPERTIES_HPP
#define CORE_PROPERTIES_HPP

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Properties.hpp>
#include <BLIB/Engine/Property.hpp>
#include <BLIB/Engine/Settings.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Constants.hpp>
#include <string>

namespace core
{
/**
 * @brief Common core application properties here. Leverages the BLIB engine configuration and
 *        properties system for serialization and global accessibility
 *
 * @ingroup Core
 */
class PropertiesStore {
public:
    BLIB_PROPERTY(DataDirectory, std::string, "app.config.data_dir", "");

    BLIB_PROPERTY(MusicPath, std::string, "blib.playlist.song_path", Constants::DefaultMusicPath);

    BLIB_PROPERTY(SpritesheetPath, std::string, "blib.animation.spritesheet_path",
                  Constants::DefaultSpritesheetPath);

    BLIB_PROPERTY(WindowWidth, unsigned int, bl::engine::Settings::WindowParameters::WindowWidthKey,
                  Constants::UnsetUintProperty);
    BLIB_PROPERTY(WindowHeight, unsigned int,
                  bl::engine::Settings::WindowParameters::WindowHeightKey,
                  Constants::UnsetUintProperty);

    BLIB_PROPERTY(UnitAiDistanceStopThresh, float, "core.unit.ai_stop_distance",
                  Constants::UnitAiStopDistanceDefault);
    BLIB_PROPERTY(UnitAiFiringMaxDistance, float, "core.unit.ai_max_shoot_distance",
                  Constants::UnitAiFiringMaxDistanceDefault);
    BLIB_PROPERTY(UnitAiFiringLineRecheckTime, float, "core.unit.ai_firing_line_recheck_time",
                  Constants::UnitAiFiringLineRecheckTimeDefault);

    BLIB_PROPERTY(UnitAccelerationDefault, float, "core.unit.default_accel",
                  Constants::UnitAccelerationDefault);
    BLIB_PROPERTY(UnitMaxSpeedDefault, float, "core.unit.default_max_speed",
                  Constants::UnitMaxSpeedDefault);
    BLIB_PROPERTY(UnitDirCorrectFactorDefault, float, "core.unit.default_dir_correct",
                  Constants::UnitDirCorrectFactorDefault);
    BLIB_PROPERTY(UnitDampingFactor, float, "core.unit.damping_factor",
                  Constants::UnitDampingFactorDefault);
    BLIB_PROPERTY(UnitRotateRateDefault, float, "core.unit.default_rotate_rate",
                  Constants::UnitRotateRateDefault);

    BLIB_PROPERTY(UnitFireRateDefault, float, "core.unit.unit_default_fire_rate",
                  Constants::UnitFireRateDefault);
    BLIB_PROPERTY(UnitBulletDamageDefault, float, "core.unit.unit_default_bullet_damage",
                  Constants::UnitBulletDamageDefault);
    BLIB_PROPERTY(UnitBulletOffsetDefault, float, "core.unit.unit_default_bullet_offset",
                  Constants::UnitBulletOffsetDefault);

    /**
     * @brief Loads the properties from the config file
     *
     * @param configFilename Base filename of the config file in Properties.DataDirectory
     * @return True if the properties could be initialized properly
     */
    bool load(const char* configFilename);

    /**
     * @brief Writes the properties to the config file
     *
     * @param configFilename Base filename of the config file in Properties.DataDirectory
     * @return True if the file could be written, false on error
     */
    bool save(const char* configFilename);
};

extern PropertiesStore Properties;

} // namespace core

#endif

#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

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

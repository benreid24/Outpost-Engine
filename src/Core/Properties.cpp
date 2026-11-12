#include <Core/Properties.hpp>

#include <BLIB/Util/FileUtil.hpp>

namespace core
{
PropertiesStore Properties;

bool PropertiesStore::load() {
    if (!bl::engine::Configuration::load()) {
        BL_LOG_WARN << "Properties file not found, using defaults";
    }
    bl::engine::Properties::syncFromConfig();
    bl::engine::Configuration::log();

    // TODO - SETUP_TASK: validate state here

    return true;
}

bool PropertiesStore::save() {
    bl::engine::Properties::syncToConfig();
    return bl::engine::Configuration::save();
}

} // namespace core

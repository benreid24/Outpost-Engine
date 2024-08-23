#include <BLIB/Logging.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Properties.hpp>
#include <Game/ConstantsGame.hpp>
#include <iostream>

namespace
{
constexpr const char* BundlePath = "data";
} // namespace

int main() {
    bl::logging::Config::rollLogs("logs", "bundle", 3);
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Info);
    bl::logging::Config::addFileOutput("logs/bundle.log", bl::logging::Config::Debug);

    if (!core::Properties.load(game::ConstantsGame::ConfigFilePath)) {
        BL_LOG_ERROR << "Failed to load game properties";
        return 1;
    }

    // TODO - SETUP_TASK - install dev resource loaders & load other game data

    using namespace bl::resource;
    constexpr auto BundleAllFiles = bundle::BundleSource::BundleAllFiles;
    constexpr auto BundleForEachRecurse =
        bundle::BundleSource::CreateBundleForEachContainedRecursive;
    constexpr auto BundleEachTopLevel = bundle::BundleSource::CreateBundleForEachContained;

    Bundler bundler(bundle::Config(BundlePath)
                        .addBundleSource({"Resources/Shaders", BundleAllFiles})
                        // TODO - SETUP_TASK - add other bundle sources and handlers
                        .addFileHandler<bundle::AnimationHandler>(".*\\.anim")
                        .withCatchAllDirectory("Resources")
                        .build());
    if (!bundler.createBundles()) {
        BL_LOG_ERROR << "Failed to create bundles";
        return 1;
    }

    BL_LOG_INFO << "Successfully bundled game resources";
    return 0;
}

#include <Sample/HitCounterManager.h>

#include "Config.h"
#include "Papyrus.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {
    constexpr std::string_view PluginName = "Sample Plugin";
    constexpr REL::Version PluginVersion = {1, 0, 0, 0};
}

#ifdef BUILD_AE
/**
 * Declaration of the plugin metadata.
 *
 * <p>
 * Modern versions of SKSE look for this static data in the DLL. It is required to have a plugin version and runtime
 * compatibility information. You should usually specify your runtime compatibility with <code>UsesAddressLibrary</code>
 * to be version independent. If you don't use Address Library you can specify specific Skyrim versions that are
 * supported.
 * </p>
 */
EXTERN_C SAMPLE_EXPORT constinit auto SKSEPlugin_Version = []() noexcept {
    SKSE::PluginVersionData v;
    v.PluginName(PluginName);
    v.PluginVersion(PluginVersion);
    v.UsesAddressLibrary(true);
    return v;
}();
#endif

/**
 * Callback used by SKSE for Skyrim runtime versions 1.5.x to detect if a DLL is an SKSE plugin.
 *
 * <p>
 * This function should set the plugin information in the plugin info and return true. For post-AE executables it is
 * never called. This implementation sets up the same information as is defined in <code>SKSEPlugin_Version</code>,
 * allowing you to still control all settings via the newer AE-compatible method while remaining backwards-compatible
 * with pre-AE SSE.
 * </p>
 *
 * <p>
 * For modern SKSE development I encourage leaving this function implemented exactly as is; in the past it was not
 * uncommon to implement functionality in this function such as initializing logging, but such logic is not compatible
 * with the post-AE initialization system. To be cross-compatible, all such logic should be in
 * <code>SKSEPlugin_Load</code>.
 * </p>
 */
EXTERN_C SAMPLE_EXPORT bool SKSEAPI SKSEPlugin_Query(const QueryInterface&, PluginInfo* pluginInfo) {
    pluginInfo->name = PluginName.data();
    pluginInfo->infoVersion = PluginInfo::kVersion;
    pluginInfo->version = PluginVersion.pack();
    return true;
}

namespace {
    /**
     * Setup logging.
     *
     * <p>
     * Logging is important to track issues. CommonLibSSE bundles functionality for spdlog, a common C++ logging
     * framework. Here we initialize it, using values from the configuration file. This includes support for a debug
     * logger that shows output in your IDE when it has a debugger attached to Skyrim, as well as a file logger which
     * writes data to the standard SKSE logging directory at <code>Documents/My Games/Skyrim Special Edition/SKSE</code>
     * (or <code>Skyrim VR</code> if you are using VR).
     * </p>
     */
    void InitializeLogging() {
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= PluginName;
        *path += L".log";

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        const auto& debugConfig = Sample::Config::GetSingleton().GetDebug();
        log->set_level(debugConfig.GetLogLevel());
        log->flush_on(debugConfig.GetFlushLevel());

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    }

    /**
     * Initialize the SKSE cosave system for our plugin.
     *
     * <p>
     * SKSE comes with a feature called a <em>cosave</em>, an additional save file kept alongside the original Skyrim
     * save file. SKSE plugins can write their own data to this file, and load it again when the save game is loaded,
     * allowing them to keep custom data along with a player's save. Each plugin must have a unique ID, which is four
     * characters long (similar to the record names used by forms in ESP files). Note however this is little-endian, so
     * technically the 'SMPL' here ends up as 'LPMS' in the save file.
     * </p>
     *
     * <p>
     * There can only be one serialization callback for save, revert (called on new game and before a load), and load
     * for the entire plugin.
     * </p>
     */
    void InitializeSerialization() {
        log::trace("Initializing cosave serialization...");
        auto* serde = GetSerializationInterface();
        serde->SetUniqueID('SMPL');
        serde->SetSaveCallback(Sample::HitCounterManager::OnGameSaved);
        serde->SetRevertCallback(Sample::HitCounterManager::OnRevert);
        serde->SetLoadCallback(Sample::HitCounterManager::OnGameLoaded);
        log::trace("Cosave serialization initialized.");
    }

    /**
     * Initialize our Papyrus extensions.
     *
     * <p>
     * A common use of SKSE is to add new Papyrus functions. You can call a registration callback to do this. This
     * callback will not necessarily be called immediately, if the Papyrus VM has not been initialized yet (in that case
     * it's execution is delayed until the VM is available).
     * </p>
     *
     * <p>
     * You can call the <code>Register</code> function as many times as you want and at any time you want to register
     * additional functions.
     * </p>
     */
    void InitializePapyrus() {
        log::trace("Initializing Papyrus binding...");
        if (GetPapyrusInterface()->Register(Sample::RegisterHitCounter)) {
            log::debug("Papyrus functions bound.");
        } else {
            log::error("Failure to register Papyrus bindings.");
        }
    }

    /**
     * Register to listen for messages.
     *
     * <p>
     * SKSE has a messaging system to allow for loosely coupled messaging. This means you don't need to know about or
     * link with a message sender to receive their messages. SKSE itself will send messages for common Skyrim lifecycle
     * events, such as when SKSE plugins are done loading, or when all ESP plugins are loaded.
     * </p>
     *
     * <p>
     * Here we register a listener for SKSE itself (because we have not specified a message source). Plugins can send
     * their own messages that other plugins can listen for as well, although that is not demonstrated in this example
     * and is not common.
     * </p>
     *
     * <p>
     * The data included in the message is provided as only a void pointer. It's type depends entirely on the type of
     * message, and some messages have no data (<code>dataLen</code> will be zero).
     * </p>
     */
    void InitializeMessaging() {
        GetMessagingInterface()->RegisterListener([](MessagingInterface::Message* message) {
            switch (message->type) {
                // Skyrim lifecycle events.
                case MessagingInterface::kPostLoad: // Called after all plugins have finished running SKSEPlugin_Load.
                    // It is now safe to do multithreaded operations, or operations against other plugins.
                    break;
                case MessagingInterface::kPostPostLoad: // Called after all kPostLoad message handlers have run.
                    break;
                case MessagingInterface::kInputLoaded: // Called when all game data has been found.
                    break;
                case MessagingInterface::kDataLoaded: // All ESM/ESL/ESP plugins have loaded, main menu is now active.
                    // It is now safe to access form data.
                    break;

                // Skyrim game events.
                case MessagingInterface::kNewGame: // Player starts a new game from main menu.
                    break;
                case MessagingInterface::kPreLoadGame: // Player selected a game to load, but it hasn't loaded yet.
                    // Data will be the name of the loaded save.
                    break;
                case MessagingInterface::kPostLoadGame: // Player's selected save game has finished loading.
                    // Data will be a boolean indicating whether the load was successful.
                    break;
                case MessagingInterface::kSaveGame: // The player has saved a game.
                    // Data will be the save name.
                    break;
                case MessagingInterface::kDeleteGame: // The player deleted a saved game from within the load menu.
                    break;
            }
        });
    }

    /**
     * Initialize the trampoline space for function hooks.
     *
     * <p>
     * Function hooks are one of the most powerful features available to SKSE developers, allowing you to replace
     * functions with your own, or replace a function call with a call to another function. However, to do this, we
     * need a code snippet that replicates the first instructions of the original code we overwrote, in order to be
     * able to call back to the original control flow with all the same functionality.
     * </p>
     *
     * <p>
     * CommonLibSSE comes with functionality to allocate trampoline space, including a common singleton space we can
     * access from anywhere. While this is not necessarily the most advanced use of trampolines and hooks, this will
     * suffice for our demo project.
     * </p>
     */
    void InitializeHooking() {
        log::trace("Initializing trampoline...");
        auto& trampoline = GetTrampoline();
        trampoline.create(64);
        log::trace("Trampoline initialized.");
        Sample::InitializeHook(trampoline);
    }
}

/**
 * This if the main callback for initializing your SKSE plugin, called just before Skyrim runs its main function.
 *
 * <p>
 * This is your main entry point to your plugin, where you should initialize everything you need. Many things can't be
 * done yet here, since Skyrim has not initialized and the Windows loader lock is not released (so don't do any
 * multithreading). But you can register to listen for messages for later stages of Skyrim startup to perform such
 * tasks.
 * </p>
 */
EXTERN_C SAMPLE_EXPORT bool SKSEAPI SKSEPlugin_Load(const LoadInterface* skse) {
    InitializeLogging();
    log::info("{} is loading...", PluginName);
    Init(skse);
    InitializeMessaging();
    InitializeSerialization();
    InitializePapyrus();
    InitializeHooking();

    log::info("{} has finished loading.", PluginName);
    return true;
}

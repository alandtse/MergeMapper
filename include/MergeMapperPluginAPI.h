#pragma once
// Interface code based on https://github.com/adamhynek/higgs

namespace MergeMapperPluginAPI {
    constexpr const auto MergeMapperPluginName = "MergeMapper";
    // Returns an IMergeMapperInterface001 object compatible with the API shown below
    // This should only be called after SKSE sends kMessage_PostLoad to your plugin
    struct IMergeMapperInterface001;
    IMergeMapperInterface001* GetMergeMapperInterface001();

    // This object provides access to MergeMapper's mod support API
    struct IMergeMapperInterface001 {
        // Gets the MergeMapper build number
        virtual unsigned int GetBuildNumber() = 0;

        /// @brief Get the new modName and formID
        /// @param oldName The original modName char* e.g., Dragonborn.esp
        /// @param oldFormID The original formID in hex format as an uint_32 e.g., 0x134ab
        /// @return a pair with char* modName and uint32 FormID. If no merge is found, it will return oldName and
        /// oldFormID.
        virtual std::pair<const char*, RE::FormID> GetNewFormID(const char* oldName,
                                                                const RE::FormID oldFormID) = 0;
    };

}  // namespace MergeMapperPluginAPI
extern MergeMapperPluginAPI::IMergeMapperInterface001* g_mergeMapperInterface;

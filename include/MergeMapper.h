#pragma once
#include "MergeMapperPluginAPI.h"
namespace MergeMapperPluginAPI {
    // Handles skse mod messages requesting to fetch API functions from MergeMapper
    void ModMessageHandler(SKSE::MessagingInterface::Message* message);

    // This object provides access to MergeMapper's mod support API version 1
    struct MergeMapperInterface001 : IMergeMapperInterface001 {
        virtual unsigned int GetBuildNumber();

        /// @brief Search the data directory for any zmerge merges. This searches for map.json files to build a mapping
        /// table.
        /// @return true if any merge mappings found
        bool GetMerges();

        /// @brief Get the new modName and formID
        /// @param oldName The original modName string e.g., Dragonborn.esp
        /// @param oldFormID The original formID in hex format as a string e.g., 0x134ab
        /// @return a pair with string modName and uint32 FormID. If no merge is found, it will return oldName and
        /// oldFormID.
        std::pair<const char*, RE::FormID> GetNewFormID(const char* oldName, const RE::FormID oldFormID);
    };
}  // namespace MergeMapperPluginAPI
extern MergeMapperPluginAPI::MergeMapperInterface001 g_interface001;

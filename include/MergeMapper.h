#pragma once
#include <detours/detours.h>

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

        /// @brief Get the original modName and formID
        /// @param newName The new merged modName char* e.g., Merge.esp
        /// @param newFormID The new merged formID in hex format as an uint_32 e.g., 0x134ab
        /// @return a pair with char* modName and uint32 FormID. If not able to find original, it will return newName
        /// and newFormID.
        std::pair<const char*, RE::FormID> GetOriginalFormID(const char* newName, const RE::FormID newFormID);

        /// @brief Whether modName is a zmerge output file. To find old file use GetOldFormID(modName, 0)
        /// @param modName The modName to check, char* e.g., merged1.esp
        /// @return true if merged
        bool isMerge(const char* modName);

        /// @brief Whether modName was an input to a zmerge file. To find new file use GetNewFormID(modName, 0)
        /// @param modName The modName to check, char* e.g., input1.esp
        /// @return true if was merged into some file
        bool wasMerged(const char* modName);
    };

    namespace Hook {
        typedef RE::TESForm*(WINAPI* pFunc)(RE::BSScript::IVirtualMachine*, std::uint32_t, std::uint32_t, RE::FormID,
                                            RE::BSString*);  // typedef to simplify signature
        inline pFunc originalFunction;
        inline RE::TESForm* replacementFunction(RE::BSScript::IVirtualMachine* a_vm, std::uint32_t a_2,
                                                std::uint32_t a_3, RE::FormID a_formID, RE::BSString* a_modname) {
            logger::debug("Game.GetFormFromFile({:x},{})", a_formID, a_modname ? a_modname->c_str() : "nullptr");
            if (g_mergeMapperInterface) {
                const auto processedFormPair = g_mergeMapperInterface->GetNewFormID(a_modname->c_str(), a_formID);
                RE::BSString newString = processedFormPair.first;
                a_formID = processedFormPair.second;
                return originalFunction(a_vm, a_2, a_3, a_formID, &newString);
            }
            return originalFunction(a_vm, a_2, a_3, a_formID, a_modname);
        }

        inline void Install() {
            const auto targetAddress = RELOCATION_ID(54832, 55465).address();
            const auto funcAddress = &replacementFunction;
            originalFunction = (pFunc)targetAddress;
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)originalFunction, (PBYTE)&replacementFunction);
            if (DetourTransactionCommit() == NO_ERROR)
                logger::info(
                    "Installed papyrus hook on GetFormFromFile_140972E10 at {0:x} with replacement from address {0:x}",
                    targetAddress, (void*)funcAddress);
            else
                logger::warn("Failed to install papyrus hook on GetFormFromFile_140972E10");
        }
    }  // namespace Hook

}  // namespace MergeMapperPluginAPI
extern MergeMapperPluginAPI::MergeMapperInterface001 g_interface001;

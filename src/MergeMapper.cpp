#include "MergeMapper.h"

#include <nlohmann/json.hpp>

#include "MergeMapperPluginAPI.h"

#define toLower(string)                                          \
    std::transform(string.begin(), string.end(), string.begin(), \
                   [](auto ch) { return static_cast<char>(std::tolower(ch)); })

namespace stl {
    using namespace SKSE::stl;
}

static nlohmann::json mergeMap;
static nlohmann::json reverseMergeMap;

using namespace MergeMapperPluginAPI;

// Interface classes are stored statically
MergeMapperInterface001 g_interface001;

// Constructs and returns an API of the revision number requested
void* GetApi(unsigned int revisionNumber) {
    switch (revisionNumber) {
        case 1:
            logger::info("Interface revision 1 requested");
            return &g_interface001;
    }
    return nullptr;
}

// Handles skse mod messages requesting to fetch API functions from VRIK
void MergeMapperPluginAPI::ModMessageHandler(SKSE::MessagingInterface::Message* message) {
    if (message->type == MergeMapperMessage::kMessage_GetInterface) {
        MergeMapperMessage* modmappermessage = (MergeMapperMessage*)message->data;
        modmappermessage->GetApiFunction = GetApi;
        logger::info("Provided MergeMapper plugin interface to {}", message->sender);
    }
}

// Fetches the MergeMapper version number
unsigned int MergeMapperInterface001::GetBuildNumber() {
    auto* plugin = SKSE::PluginDeclaration::GetSingleton();
    return plugin->GetVersion().pack();
}

std::uint32_t parseMergeLog(const std::wstring a_path, const std::string mergedPlugin) {
    std::string line;
    const std::string pluginStart = "Copying records from ";
    const std::string formIDStart = "Copying ";
    std::string originalPlugin = "";
    RE::FormID formID;
    std::string sFormID;
    std::uint32_t count = 0;
    auto mergedPluginKey = mergedPlugin;
    // lowercase mergedPlugin since it is also a key for reversemap
    toLower(mergedPluginKey);
    auto constexpr mergePrefix = L"merge_";
    for (const auto& entry : std::filesystem::directory_iterator(a_path)) {
        const auto& path = entry.path();
        const auto& file = path.filename();
        if (entry.exists() && entry.is_regular_file() && file.wstring().starts_with(mergePrefix) &&
            file.extension().string() == ".txt") {
            logger::debug("mergedPlugin: Processing {}", path.string());
            try {
                std::ifstream input(path);
                while (std::getline(input, line)) {
                    const auto closeBracketPos = line.find("]");
                    if (line.starts_with(pluginStart)) {
                        // unlike mergedPlugin, originalPlugin is iterated against instead of used as a key so no need
                        // to lowercase
                        originalPlugin = line.substr(pluginStart.length());
                        logger::debug("\tFound processing record for {} from {}", originalPlugin, line);
                    } else if (!originalPlugin.empty() && line.starts_with(formIDStart) &&
                               closeBracketPos != std::string::npos) {
                        count++;
                        // parse formid from log, e.g., 1C001841 from Copying WodoWigQuestLeafAngelic [DLBR:1C001841]
                        formID = std::stoi(line.substr(closeBracketPos - 8, 8), 0, 16) & 0x00FFFFFF;
                        sFormID = std::format("{:x}"sv, formID);
                        toLower(sFormID);
                        reverseMergeMap[mergedPluginKey][originalPlugin][sFormID] = sFormID;
                        logger::debug("\tStored value {} at reverseMergedMap[{}][{}][{}] from {}",
                                      reverseMergeMap[mergedPluginKey][originalPlugin][sFormID], mergedPluginKey, originalPlugin,
                                      sFormID,
                                      line);
                    }
                    continue;
                }
                input.close();
            } catch (std::exception& e) {
                logger::warn("	Unable to open {}:{}",
                             stl::utf16_to_utf8(path.wstring()).value_or("<unicode conversion error>"s), e.what());
            }
        }
    }
    return count;
}

bool MergeMapperInterface001::GetMerges() {
    using json = nlohmann::json;
    logger::info("Searching for merges within the Data folder");
    auto constexpr folder = R"(Data\)";
    json json_data;
    size_t total = 0;
    size_t reverseMapTotal = 0;
    auto constexpr mergePrefix = L"Data\\merge - ";
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        // zMerge folders have name "merge - 018auri"
        std::wstring path = L"";
        try {
            path = entry.path().wstring();
        } catch (std::exception& e) {
            logger::warn("	Unable to convert path to string: {}", e.what());
        }
        if (entry.exists() && entry.is_directory() && path.starts_with(mergePrefix)) {
            auto file = path + L"\\merge.json";
            auto espPath = path.substr(13) + L".esp";
            try {
                std::ifstream json_file(file);
                json_file >> json_data;
                json_file.close();
                auto filename = json_data["filename"].get<std::string>();
                espPath = std::wstring(filename.begin(), filename.end());
            } catch (std::exception& e) {
                logger::warn("	Unable to open {}, defaulting filename to {}:{}",
                             stl::utf16_to_utf8(file).value_or("<unicode conversion error>"s),
                             stl::utf16_to_utf8(espPath).value_or("<unicode conversion error>"s), e.what());
            }
            file = path + L"\\map.json";
            try {
                std::ifstream json_file(file);
                json_file >> json_data;
                json_file.close();
            } catch (std::exception& e) {
                logger::warn("	Unable to open {}:{}", stl::utf16_to_utf8(file).value_or("<unicode conversion error>"s),
                             e.what());
            }
            // json requires wstring conversion to utf encoding
            // https://json.nlohmann.me/home/faq/#parse-errors-reading-non-ascii-characters
            auto mergedPlugin = stl::utf16_to_utf8(espPath).value_or(""s); // the final merged plugin that contains original plugins
            if (!std::filesystem::exists(folder + mergedPlugin)) {
                logger::warn("	{} does not exist, not processing merges for this file", mergedPlugin);
                continue;
            }
            auto count = 0;
            auto reverseMapCount = 0;
            if (mergedPlugin != "" && !json_data.empty()) {
                reverseMapCount += parseMergeLog(path, mergedPlugin);
                for (auto& [originalPlugin, idmap] : json_data.items()) {
                    auto originalPluginKey = originalPlugin; // key is lowercase since we search on it
                    toLower(originalPluginKey);
                    if (mergeMap.contains(originalPluginKey))
                        logger::warn("\tDuplicate {} found merged in {}; one can be removed", originalPlugin,
                                     mergedPlugin);
                    auto mergedPluginKey = mergedPlugin;
                    // lowercase mergedPlugin since it is also a key for reversemap
                    toLower(mergedPluginKey);
                    // store original name
                    mergeMap[originalPluginKey]["name"] = mergedPlugin;
                    for (auto& [sOriginalFormID, sNewFormID] : idmap.items()) {
                        auto storedKey = std::format("{:x}"sv, std::stoi(sOriginalFormID, 0, 16));
                        toLower(storedKey);
                        auto storedValue = std::format("{:x}"sv, std::stoi(sNewFormID.get<std::string>(), 0, 16));
                        toLower(storedValue);
                        mergeMap[originalPluginKey]["map"][storedKey] = storedValue;
                        reverseMergeMap[mergedPluginKey][originalPlugin][storedValue] = storedKey;
                        logger::debug("\tStored mapped value {} at reverseMergedMap[{}][{}][{}]",
                                      reverseMergeMap[mergedPluginKey][originalPlugin][storedValue], mergedPluginKey,
                                        originalPlugin,
                                        storedValue);
                    }
                    count += idmap.size();
                    logger::info(" Found {} maps to {} with {} mappings and {} reverse mappings", originalPlugin, mergedPlugin, count, reverseMapCount);
                    total += count;
                    reverseMapTotal += reverseMapCount;
                }
            }
        }
    }
    if (mergeMap.empty()) {
        logger::info("\tNo merges were found within the Data folder");
        return false;
    }
    logger::info("\t{} merges found with {} mappings and {} reverse mappings", mergeMap.size(), total, reverseMapTotal);
    return true;
}
std::pair<const char*, RE::FormID> GetNewFormID(std::wstring& oldName, const RE::FormID oldFormID) {
    auto converted_oldName = stl::utf16_to_utf8(oldName).value_or(
        ""s);  // json requires wstring conversion to utf
               // https://json.nlohmann.me/home/faq/#parse-errors-reading-non-ascii-characters
    if (converted_oldName == "") logger::error(" Unable to convert oldName to UTF encoding; no mapping possible");
    return std::make_pair(converted_oldName.c_str(), oldFormID);
}

std::pair<const char*, RE::FormID> MergeMapperInterface001::GetNewFormID(const char* oldName,
                                                                         const RE::FormID oldFormID) {
    const char* modName = oldName;
    std::string espkey = oldName;
    toLower(espkey);
    RE::FormID formID = oldFormID;
    // check for merged esps
    if (mergeMap.contains(espkey)) {
        modName = mergeMap[espkey]["name"].get_ptr<nlohmann::json::string_t*>()->c_str();
        auto storedKey = std::format("{:x}"sv, formID);
        if (!mergeMap[espkey]["map"].empty()) {
            toLower(storedKey);
            if (mergeMap[espkey]["map"].contains(storedKey)) {
                formID = std::stoi(mergeMap[espkey]["map"][storedKey].get<std::string>(), 0, 16);
            }
        }
    }
    if (oldName != modName && oldFormID != formID)
        logger::debug("GetNewFormID:\t{} {:x} -> {} {:x}", oldName, oldFormID, modName, formID);
    else if (oldName != modName)
        logger::debug("GetNewFormID:\t{} -> {} {:x}", oldName, modName, formID);
    else if (oldFormID != formID)
        logger::debug("GetNewFormID:\t{} {:x} -> {:x}", oldName, oldFormID, formID);
    return std::make_pair(modName, formID);
}

std::pair<const char*, RE::FormID> MergeMapperInterface001::GetOriginalFormID(const char* newName,
                                                                              const RE::FormID newFormID) {
    const char* modName = newName;
    RE::FormID formID = newFormID;
    std::string mergedPluginKey{newName};
    // lowercase mergedPlugin since it is also a key for reverseMergeMap
    toLower(mergedPluginKey);
    if (reverseMergeMap.contains(mergedPluginKey)) {
        for (auto& [originalPlugin, value] : reverseMergeMap[mergedPluginKey].items()) {
            auto sFormID = std::format("{:x}"sv, formID);
            toLower(sFormID);
            if (value.contains(sFormID)) {
                modName = originalPlugin.c_str();
                formID = std::stoi(value[sFormID].get<std::string>(), 0, 16);
                break;
            }
        }
    }
    if (newName != modName && newFormID != formID)
        logger::debug("GetOriginalFormID:\t{} {:x} -> {} {:x}", newName, newFormID, modName, formID);
    else if (newName != modName)
        logger::debug("GetOriginalFormID:\t{} -> {} {:x}", newName, modName, formID);
    else if (newFormID != formID)
        logger::debug("GetOriginalFormID:\t{} {:x} -> {:x}", newName, newFormID, formID);
    return std::make_pair(modName, formID);
}
#include "MergeMapper.h"
#include <nlohmann/json.hpp>


namespace stl {
    using namespace SKSE::stl;
}

static nlohmann::json mergeMap;

using namespace MergeMapperPluginAPI;

// A message used to fetch MergeMapper's interface
struct MergeMapperMessage {
    enum : uint32_t { kMessage_GetInterface = 0xe6cb8b59 };  // Randomly generated
    void* (*GetApiFunction)(unsigned int revisionNumber) = nullptr;
};

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


bool MergeMapperInterface001::GetMerges() {
    using json = nlohmann::json;
    logger::info("Searching for merges within the Data folder");
    auto constexpr folder = R"(Data\)";
    json json_data;
    size_t total = 0;
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
            auto merged = path.substr(13) + L".esp";
            try {
                std::ifstream json_file(file);
                json_file >> json_data;
                json_file.close();
                auto filename = json_data["filename"].get<std::string>();
                merged = std::wstring(filename.begin(), filename.end());
            } catch (std::exception& e) {
                logger::warn("	Unable to open {}, defaulting filename to {}:{}",
                                stl::utf16_to_utf8(file).value_or("<unicode conversion error>"s),
                                stl::utf16_to_utf8(merged).value_or("<unicode conversion error>"s), e.what());
            }
            file = path + L"\\map.json";
            try {
                std::ifstream json_file(file);
                json_file >> json_data;
                json_file.close();
            } catch (std::exception& e) {
                logger::warn("	Unable to open {}:{}",
                                stl::utf16_to_utf8(file).value_or("<unicode conversion error>"s), e.what());
            }
            auto converted_merged = stl::utf16_to_utf8(merged).value_or(
                ""s);  // json requires wstring conversion to utf encoding
                        // https://json.nlohmann.me/home/faq/#parse-errors-reading-non-ascii-characters
            if (!std::filesystem::exists(folder + converted_merged)) {
                logger::warn("	{} does not exist, not processing merges for this file", converted_merged);
                continue;
            }
            if (converted_merged != "" && !json_data.empty()) {
                for (auto& [esp, idmap] : json_data.items()) {
                    auto espkey = esp;
                    std::transform(espkey.begin(), espkey.end(), espkey.begin(),
                                    [](auto ch) { return static_cast<char>(std::tolower(ch)); });
                    if (idmap.size()) {
                        logger::info(" Found {} maps to {} with {} mappings", esp, converted_merged, idmap.size());
                        total += idmap.size();
                    }
                    if (mergeMap.contains(espkey)) logger::warn(" Duplicate {} found in {}", esp, converted_merged);
                    mergeMap[espkey]["name"] = converted_merged;
                    if (!idmap.empty()) {
                        for (auto& [key, value] : idmap.items()) {
                            auto storedKey = std::to_string(std::stoi(key, 0, 16));
                            std::transform(storedKey.begin(), storedKey.end(), storedKey.begin(),
                                            [](auto ch) { return static_cast<char>(std::tolower(ch)); });
                            auto storedValue = std::to_string(std::stoi(value.get<std::string>(), 0, 16));
                            std::transform(storedValue.begin(), storedValue.end(), storedValue.begin(),
                                            [](auto ch) { return static_cast<char>(std::tolower(ch)); });
                            mergeMap[espkey]["map"][storedKey] = storedValue;
                        }
                    }
                }
            }
        }
    }
    if (mergeMap.empty()) {
        logger::info("	No merges were found within the Data folder");
        return false;
    }
    logger::info("	{} merges found with {} mappings", mergeMap.size(), total);
    return true;
}
std::pair<const char*, RE::FormID> GetNewFormID(std::wstring& oldName, const RE::FormID oldFormID) {
	auto converted_oldName = stl::utf16_to_utf8(oldName).value_or(""s);  //json requires wstring conversion to utf https://json.nlohmann.me/home/faq/#parse-errors-reading-non-ascii-characters
	if (converted_oldName == "")
		logger::error(" Unable to convert oldName to UTF encoding; no mapping possible");
	return std::make_pair(converted_oldName.c_str(), oldFormID);
}

std::pair<const char*, RE::FormID> MergeMapperInterface001::GetNewFormID(const char* oldName,
                                                                         const RE::FormID oldFormID) {
    const char* modName = oldName;
    std::string espkey = oldName;
	std::transform(espkey.begin(), espkey.end(), espkey.begin(), [](auto ch) { return static_cast<char>(std::tolower(ch)); });
	RE::FormID formID = oldFormID;
	//check for merged esps
	if (mergeMap.contains(espkey)) {
        modName = mergeMap[espkey]["name"].get_ptr<nlohmann::json::string_t*>()->c_str();
		auto storedKey = std::to_string(formID);
		if (!mergeMap[espkey]["map"].empty()) {
			std::transform(storedKey.begin(), storedKey.end(), storedKey.begin(), [](auto ch) { return static_cast<char>(std::tolower(ch)); });
			if (mergeMap[espkey]["map"].contains(storedKey)) {
				formID = std::stoi(mergeMap[espkey]["map"][storedKey].get<std::string>());
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


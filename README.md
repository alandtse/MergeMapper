# MergeMapper SKSE Plugin
A SKSE plugin that maps zmerged plugins. This is intended to be used by other SKSE dlls. Built on CommonlibNG.
* [Nexus](https://www.nexusmods.com/skyrimspecialedition/mods/74689)

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG)

## User Requirements
* [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
	* Needed for SSE/AE
* [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
	* Needed for VR

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/alandtse/MergeMapper.git
# Open with Visual Studio
```

## License
[Apache-2.0](LICENSE)

## Credits
* Built using [CommonLibSSE NG Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin).
* SKSE Interface code from [higgs](https://github.com/adamhynek/higgs).

## Developer Use of MergeMapper

### General
1. Copy [include/MergeMapperPluginAPI.h](include/MergeMapperPluginAPI.h) and [src/MergeMapperPluginAPI.cpp](src/MergeMapperPluginAPI.cpp) into your project.
   * Alternatively use [vcpkg to manage](#manage-dependency-with-vcpkg)
2. Get API doing SKSE PostPostLoad. By default it is in the global `g_mergeMapperInterface`.
```cpp
#include "MergeMapperPluginAPI.h"
...
void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	if (a_message->type == SKSE::MessagingInterface::kPostPostLoad) {
		MergeMapperPluginAPI::GetMergeMapperInterface001();  // Request interface
		if (g_mergeMapperInterface) { // Use Interface
			const auto version = g_mergeMapperInterface->GetBuildNumber();
			logger::info("Got MergeMapper interface buildnumber {}", version);
		}else
			logger::info("MergeMapper not detected");
	}
}
```
3. Use interface to query Mod/formID info.
```cpp
// From Base Object Swapper https://github.com/powerof3/BaseObjectSwapper
			const auto formPair = string::split(a_str, "~"); // splits "FormID~modName" e.g. 0x10C0E3~Skyrim.esm
			if (g_mergeMapperInterface){
				const auto [modName, formID] = g_mergeMapperInterface->GetNewFormID(formPair[1].c_str(), std::stoi(formPair[0], 0, 16));
				return RE::TESDataHandler::GetSingleton()->LookupFormID(formID, (const char*) modName);
			}else{
				return RE::TESDataHandler::GetSingleton()->LookupFormID(std::stoi(formPair[0], 0, 16), formPair[1]);
      }
```

### Manage dependency with vcpkg

VCPKG is supported using a custom port found in [cmake/ports/mergemapper](cmake/ports/mergemapper).

1. Edit vcpkg.json to add mergemapper.
```json
{
  ...
  "dependencies": [
    "mergemapper",
    ...
  ]
}
```
2. Ensure cmake support [custom ports](https://github.com/microsoft/vcpkg/blob/master/docs/users/config-environment.md#vcpkg_overlay_ports). Example for setting `CmakePresets.json`.
```json
        "VCPKG_OVERLAY_PORTS": {
          "type": "STRING",
          "value": "${sourceDir}/cmake/ports/"
        }
```
3. Copy [cmake/ports/mergemapper](cmake/ports/mergemapper) to `/cmake/ports` in your project.
4. Modify CMakeFiles.txt to include the files.
```cmake
#...
find_path(MERGEMAPPER_INCLUDE_DIRS "MergeMapperPluginAPI.h")
#...
add_library(
	${PROJECT_NAME}
	SHARED
	${MERGEMAPPER_INCLUDE_DIRS}/MergeMapperPluginAPI.cpp
)
#...
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
    #...
		${MERGEMAPPER_INCLUDE_DIRS}
)
```

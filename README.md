# Fully Dynamic Game Engine C++ Sample Plugin
A sample SKSE plugin developed in C++, built on the Fully Dynamic Game Engine platform for Skyrim Special Edition.

## Table of Contents
* [Getting Started](#getting-started)
  * [Environment Setup](#environment-setup)
    * [Installing Visual Studio](#installing-visual-studio)
    * [Installing Git](#installing-git)
    * [Vcpkg Install and Configuration](#vcpkg-install-and-configuration)
  * [Cloning the Repository](#cloning-the-repository)
    * [Importing the Project into Your IDE](#importing-the-project-into-your-ide)
      * [Visual Studio](#visual-studio)
      * [Visual Studio Code](#visual-studio-code)
* [Understanding the Project Structure](#understanding-the-project-structure)
  * [Vcpkg Setup](#vcpkg-setup)
  * [CMake](#cmake)
  * [Papyrus Development](#papyrus-development)

## Getting Started
### Environment Setup
#### Installing Visual Studio
To do Windows development you will need to install [Visual Studio](https://visualstudio.microsoft.com/). The Community
Edition is free to install, but you must create a Visual Studio account. During install you will be presented with
the components you wish to install for development. The only one required for SKSE development is "Desktop development
with C++". Select it and leave the detailed options on the right untouched unless you really know what you are doing.

![Visual Studio Installer](docs/visual-studio-install.png)

#### Installing/Configuring Visual Studio Code
The Visual Studio installer includes the Visual Studio IDE as well as the development tools needed for C++ development.
However, many SKSE developers use Papyrus as well, since SKSE can be used to add new Papyrus bindings. This is typically
done using Visual Studio Code as it has advanced plugins for Papyrus development other IDEs lack. You can [download
Visual Studio Code](https://visualstudio.microsoft.com/) for free at the same site you used to get Visual Studio.

Once installed, open Visual Studio Code. On the left-hand side of the window find the button called Extensions and click
it (or press `Ctrl+Shift+X`). Search for "Papyrus" in the search bar at the top of the panel and find the extension
called "Papyrus" by Joel Day. Click "Install" to install the extension. You will now be able to setup Papyrus
development workspaces in VS Code.

![Visual Studio Code Papyrus Extension Install](docs/vscode-papyrus-install.png)

#### Installing Git
If you do not already have Git installed, [download and install it](https://gitforwindows.org/) (you do not need to
worry about the specific configuration options during install).

#### Vcpkg Install and Configuration
Vcpkg is a package manager for C/C++ libraries, which makes integrating third-party libraries into your project easy. It
is also installed with Git. Clone Vcpkg and then set it up by running the following commands:

```commandline
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg integrate install
```

This project allows for using default Vcpkg configuration when none is externally specified (e.g. from the command line
or when built as a dependency via Vcpkg). This makes development in your dev environment simpler. To auto-detect Vcpkg
you must set an environment variable `VCPKG_ROOT` to the path to your Vcpkg install. To do so open Control Panel and
go to System. On the left-hand side click About. You will now see an option on the right-hand side of the window for
"Advanced system settings". You will get a window with a number of options; click "Environment Variables".

![Environment Variables Button](docs/vcpkg-system-properties.png)

In the environment variables screen click New and enter `VCPKG_ROOT` as the variable name. For the value, enter the full
path to your Vcpkg install. Note that this variable is not picked up by currently-running applications, until they are
restarted.

![Environment Variables Settings](docs/vcpkg-env.png)

### Cloning the Repository
Clone this repository to your local machine by running
the following command at the command line (using Command Prompt, Powershell, or Windows Terminal):

```commandline
git clone https://gitlab.com/colorglass/fudge-sample-plugin-cpp.git
```

This will create a directory called `fudge-sample-plugin-cpp` with a clone of this project.

### Importing the Project into Your IDE
#### Visual Studio
Open Visual Studio. You will be presented with a launch screen that lets you select the project to work on. You want to
select "Open a local folder".

![Visual Studio Import](docs/visual-studio-import-folder.png)

Find the folder to which you cloned this repository and select that folder. Visual Studio should open and you will
shortly see a screen that should look roughly like the following (note your window may vary in the visual theme, icons,
and placement of the panels, such as the file tree being on the right side rather than left; this is because my own
installation I used to take these screenshots is not fresh and has been customized).

![Visual Studio Project Imported](docs/visual-studio-freshly-imported.png)

Visual Studio will begin to import the CMake configuration. CMake is build system used by this project, and it is
configured in the `CMakeLists.txt` file. Visual Studio will automatically begin to download and build all the
dependencies needed to build this project, and import the projects settings. This will take some time. If you do not
see the "Output" panel, look around the edge of the screen for a tab that says "Output" and click it to see the output
from the CMake import process. Wait until it is done. If you open files before it is complete Visual Studio cannot tell
you yet if there are any problems, and you will see a warning along the top of the file that C++ Intellisense is not yet
available.

![Visual Studio CMake Import In Progress](docs/visual-studio-cmake-processing.png)

Once Visual Studio has completed the import of the project, you can now do development. You will now have an option to
build the project under the Build menu. Use `Build->Build All` (or `Ctrl+Shift+B`) to build the project.

![Visual Studio CMake Import In Progress](docs/visual-studio-cmake-success.png)

Build the project with `Build->Build All` (or `Ctrl+Shift+B`); if all has gone well you should see a notification that
the build was successful in your status bar, and in the output panel if it is visible. Congratulations, you've built
your first SKSE plugin! You can find the DLL in the project directory under `build/FDGESamplePlugin.dll`.

![Visual Studio CMake Import In Progress](docs/visual-studio-build-success.png)

#### Visual Studio Code

## Understanding the Project Structure
###  Vcpkg Setup
To properly link against the dependencies you will need a custom Vcpkg triplet. Triplets define the target platform and
whether a library is linked against statically or dynamically. For SKSE development it is important that most libraries
be linked against statically; if linked dynamically those libraries' DLLs must be included in the SKSE plugins
directory, which will generate a lot of noise in SKSE log files warning about DLLs being found which are not SKSE
plugins.

There are however exceptions to this rule. If you are directly linking against SKSE (not recommended for modern
SKSE development and not done this project), or linking against another SKSE plugin that is acting as a library (such as
Fully Dynamic Game Engine is used here), those should be linked dynamically. To handle this a custom triplet has been
placed in the `cmake` directory, called `x64-windows-skse`, which implements such exceptions. The CMake definition
adds this directory as a place to find triplets via the `VCPKG_OVERLAY_TRIPLETS` property.

In addition, to use the custom dependencies not found in the official Vcpkg repository, a custom repository has been
defined in `vcpkg-configuration.json`. This includes Fully Dynamic Game Engine and its dependencies, including a port of
CommonLibSSE, which thanks to this repository can now be consumed as a Vcpkg package.

Finally, we have defined this project as a Vcpkg in `vcpkg.json`. This includes our dependencies and other
configuration. Of note here is that this project is defined to have multiple *features*. The core feature is the SKSE
plugin, but in addition two more features are defined: one to build the tests, and one to distribute the Papyrus scripts
defined in this project. If your plugin is intended to be consumed by others, then this allows you to exclude tests from
the version that third-parties will install, as well as distribute your Papyrus script sources to them to compile their
own scripts against.

### CMake
#### Vcpkg Configuration
Vcpkg and the target triplet/linking behavior is automatically configured for this project. When Vcpkg configuration is
not explicit when running CMake, the Vcpkg toolchain file is discovered via the environment variable `VCPKG_ROOT`. This
variable is commonly used in many open source projects to detect and automatically configure their projects, and this
plugin follows that convention. This simplifies your IDE setup by not requiring manual configuration. The correct
triplet is also configured (see [Vcpkg Setup](#vcpkg-setup) for more information), and static linking is configured
using either the debug or release MSVC runtimes, depending on whether the project is built in debug or release mode.

```cmake
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    cmake_path(SET VcpkgSrc $ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "${VcpkgSrc}/scripts/buildsystems/vcpkg.cmake")
endif ()
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-skse")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

#### Build Options

#### Post-Build Automatic Deployment
The CMake build includes an optional post-build auto-deployment feature. If you have set an environment variable
`FDGESampleTarget` then the compiled DLL and it's debugging symbols (the PDB file) will be copied to that directory.
This can be a path to your `Data/SKSE/Plugins` directory or the equivalent under a Mod Organizer 2 mod to auto-update
your plugin for testing after every build.

```cmake
if (WIN32 AND DEFINED ENV{${PROJECT_NAME}Target})
  message("Adding build copy target $ENV{${PROJECT_NAME}Target}.")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> $ENV{${PROJECT_NAME}Target}
      )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> $ENV{${PROJECT_NAME}Target}
      )
endif ()
```

Note that if you are building both release and debug configurations whichever completes second will be the one that ends
up in the target directory.

### Plugin Declaration
The plugin is declared in `src/Main.cpp`. Fully Dynamic Game Engine has a declarative plugin definition system. This is
based on the system used by SKSE in the post-AE versions. Unlike raw SKSE plugins it is not required to assign it via an
explicit lambda execution, and no `SKSEPlugin_Load` function is required.

```c++
SKSEPlugin(
        Name = "Sample Plugin";
        Version = {1, 0, 0};
        Author = "Charmed Baryon";
        Email = "charmedbaryon@elderscrolls-ng.com";
        PreInit = []() {
            FDGE::Logger::Initialize(Config::GetProxy());
            Config::GetProxy().StartWatching();
        };
);
```

Most properties correspond to those in the existing SKSE plugin definition. The `SKSESerializationPluginID` defines the
record (the four characters) that will be used to save this plugin's data in the SKSE cosave, when using SKSE's save
game integration. The `PreInit` property is a function that will be run immediately upon plugin initialization, before
the rest of the initialization takes place. It is primarily for initializing logging systems so that logging can already
be present to log the initialization process itself.

### Config File Support
The configuration for this plugin is defined in `src/Config.h`. FuDGE provides a powerful and exensible config system
that can source data from files or any other data source (if an implementation exists). The bridge between a data source
and the data is called a "proxy". This example plugin uses the `DynamicFileProxy`, which is given a file name without an
extension and looks for a config file with that base name with one of its supported extensions. Based on the extension
it finds it will determine the expected file format. This lets the plugin support configuration in multiple formats,
such as YAML, JSON, TOML, and others.

The framework which reads the config file is, by default, Articuno. Articuno is a powerful serialization framework that
can read data even into fully functioning (non-POCO, for C++ techies) class structures. In this case we are using the
default base class for plugin configuration from FuDGE, which comes with logging configuration. FuDGE provides a
powerful logging system that can be used with this logging configuration.

FuDGE focuses heavily on being dynamic, and configs are no exception. FuDGE's config system can react to changes to the
config file at runtime. We see this being enabled in the `PreInit` function of the plugin declaration, where we call
`StartWatching` on the config proxy. This makes the proxy watch the underlying file for changes. When a change happens
an event is emitted, and event listeners can update the various plugin systems in reaction.

```c++
PreInit = []() {
    FDGE::Logger::Initialize(Config::GetProxy());
    Config::GetProxy().StartWatching();
};
```

### Logging
As mentioned above, FuDGE has a logging system that can be initialized if basing a plugin's configuration on that of
FuDGE's base config class. We see this being initialized in the `PreInit` function of the plugin declaration in
`src/Main.cpp`.

```c++
PreInit = []() {
    FDGE::Logger::Initialize(Config::GetProxy());
    Config::GetProxy().StartWatching();
};
```

Calling `FDGE::Logger::Initialize` starts the logging system. This should be done in `PreInit` to get logging for the
initialization process itself. The FuDGE logging system supports file logging (generally to `Documents\My Games\Skyrim
Special Edition\SKSE`, the standard location for SKSE plugin logs), debug logging (which will output logs to your IDE
when you have attached a debugger), and Windows event logging (viewable in the Windows Event Viewer). A config file is
included at `contrib/FDGESamplePlugin.yaml` that explicitly lists all the default values for logging, but note that if
the config file is absent or any values are not included in the config file the default value is assumed.

The logging system is fully dynamic. If you have called `StartWatching` on the proxy, the logger will react to update
events and reconfigure itself, allowing the log configuration to be changed at runtime.

### Papyrus Bindings and Script Objects

### Declarative Event Handling

### Save Game Hooks

### Function/Call Hooks

### Custom Console Commands

### Unit Testing
If unit tests are enabled in the build, then a test executable will be created.

```cmake
if (BUILD_TESTS)
  enable_testing()

  add_executable(
          ${PROJECT_NAME}Tests
          ${headers}
          ${tests}
  )

  target_compile_features(${PROJECT_NAME}Tests
          PUBLIC
          cxx_std_23
          )

  target_link_libraries(
          ${PROJECT_NAME}Tests
          PRIVATE
          ${PROJECT_NAME}
          GTest::gmock
          GTest::gtest
          GTest::gmock_main
          GTest::gtest_main
  )

  target_precompile_headers(${PROJECT_NAME}Tests
          PRIVATE
          src/PCH.h
          )

  gtest_discover_tests(${PROJECT_NAME}Tests)
endif ()
```

Tests are built on the Google Test framework, and dummy sample tests are included under the `test` directory. If you
add a new test file you must add it to the CMake definition for `tests`. The output will be `FDGESamplePluginTests.exe`
and running that executable runs the tests. Google Test support is built into Visual Studio and other IDEs such as
CLion, and therefore can be run directly in the IDE to see test results.

### Papyrus Development
The sample project includes configuration for a Visual Studio Code workspace that can utilize the Papyrus plugin. It is
setup for the automated Papyrus script extraction done via Vcpkg using the custom repository, and therefore can compile
against Papyrus sources from vanilla Skyrim, SKSE, and Fully Dynamic Game Engine.

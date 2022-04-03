# CommonLibSSE Sample Plugin
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

## Understanding the Project
### Build Features
#### Vcpkg Integration
Like many SKSE projects, this sample project uses Vcpkg to manage dependencies that are needed to build and run the
project. However one advanced feature seen here is the use of Vcpkg to manage even Skyrim-oriented dependencies.
Traditionally projects like CommonLibSSE were included via a Git submodule. This has a number of disadvantages. It
subjects CommonLibSSE to the build configuration for your project. It also requires you to list all the transitive
dependencies in your own `vcpkg.json` file.

To solve this problem the Skyrim NG project has produced a public repository, available for all in the Skyrim and
Fallout 4 communities, to use for their development. This repository includes the modern head of CommonLibSSE
development (called simply `commonlibsse`), as well as a legacy build that has modern features but maintains
compatibility with pre-AE versions of Skyrim SE (called `commonlibsse-legacy`, and based on powerof3's fork of
CommonLibSSE), and a modernized version of CommonLibVR that has been adapted for CMake builds (based on alandtse's
fork of CommonLibVR).

```json
{
    "registries": [
        {
            "kind": "git",
            "repository": "https://gitlab.com/colorglass/vcpkg-colorglass",
            "baseline": "926654b35b42c50f8f6928c60d76acc4b0a20213",
            "packages": [
              // ...
            ]
        }
    ]
}

```

Furthermore, this Vcpkg repository includes the ability to build and link to SKSE itself, as well as the ability to
deploy the original Bethesda script sources and SKSE versions of those sources. Using the `bethesda-skyrim-scripts`
port will cause Vcpkg to find your Skyrim installation via the registry and extract the script sources locally into
your project build directory, allowing you to do local Papyrus development. SKSE scripts are also download and extracted
when using the `skse` port's `scripts` feature.

The availability of these projects is handled by the `vcpkg-configuration.json` file, which brings in the Skyrim NG
repository hosted by Color-Glass Studios, and is a big step forward in streamlining the development process.

#### Multi-Runtime Builds
A major problem with developing for modern Skyrim is the fragmentation of Skyrim runtimes between pre-AE executables,
post-AE executables, and Skyrim VR. This project demonstrates how to achieve support for all three in a single codebase.
Through the Vcpkg and CMake configuration, there can be SE/AE/VR variations of the plugin built. The separation fo these
features is handled in `vcpkg.json`, where each version of the plugin uses a different version of the CommonLibSSE ports
(`commonlibsse` for AE, `commonlibsse-legacy` for SE, and `commonlibvr` for VR). The source code for the project
otherwise remains the same, save for the different address library IDs between them, as found in `Papyrus.cpp`:

```c++
#ifdef BUILD_AE
    REL::ID id(44001);
#elif BUILD_SE
    REL::ID id(42832);
#elif BUILD_VR
    REL::ID id(0); // TODO: Find ID for VR.
#else
    static_assert(false, "The build must target Skyrim AE, SE, or VR.");
#endif
```

Note that some projects can achieve clearer portability (e.g. if using Fully Dynamic Game Engine it is possible to have
not only one codebase, but a single DLL, with all three IDs defined in one line, and the runtime dynamically selects the
correct one). However, this sample project shows strict use of only CommonLibSSE and minimal additional dependencies.

The correct Vcpkg features are chosen via the CMake build profile. Build profiles are defined in the `CMakePresets.json`
file, which map build types (e.g. `Debug-AE`) to the proper set of Vcpkg features (e.g. `plugin-ae`).

#### Automatic Deployment
When building the sample project, build results are automatically deployed to `contrib/Distribution`. This directory
has the FOMOD installer for the project. DLL and PDB files are copied automatically to the appropriate directory for the
build type and target runtime (AE, SE, or VR). In addition, the CMake clean action has been extended to clean this files
in the FOMOD directory. The project also integrates with the Papyrus extension for Visual Studio Code. When performing a
build of the Papyrus scripts the result will be copied to the appropriate directory for Papyrus scripts (the compiled
scripts are also cleaned by a CMake clean).

You can also incrementally build to a mod directory in Mod Organizer 2. The CMake build is configured to deploy the DLL
and PDB files to an MO2 mod directory if one has been specified by environment variables. These variables are called
`CommonLibSSESamplePluginTargetAE`, `CommonLibSSESamplePluginTargetSE`, and `CommonLibSSESamplePluginTargetVR` for AE,
SE, and VR targets respectively. These should be set to point to the base directory for the MO2 mod you want to deploy
the files to (do not include `SKSE/Plugins` at the end). This allows you to simply build after making changes and
immediately be able to run Skyrim from MO2 to see the results.

#### Unit Testing
The project comes with built-in support for running unit tests with GTest. The build produces an executable with all
GTest unit tests; running this executable will run the tests. See `test/HitCounterManagerTest.cpp` for an example. GTest
is the most widely used unit testing framework with wide support integrated into IDEs, including support by Visual
Studio, Visual Studio Code, and CLion.

#### Miscellaneous Elements
The CMake configuration for the project addresses common issues with C++ development.

```cmake
  add_compile_definitions(
          UNICODE
          _UNICODE
          NOMINMAX
          _AMD64_
          WIN32_LEAN_AND_MEAN
          _CRT_USE_BUILTIN_OFFSETOF # Fixes MSVC being non-compliant with offsetof behavior by default.
  )

  if ($ENV{CLION_IDE})
      add_compile_definitions(
              __cpp_lib_char8_t         # Workaround for CLion bug.
              __cpp_consteval           # Workaround for CLion bug.
      )
  endif ()
```

Interprocedural optimizations are enabled whenever possible, which improves performance by further optimizing the
output at link-time:

```cmake
check_ipo_supported(RESULT USE_IPO OUTPUT IPO_OUTPUT)
if (USE_IPO)
    message("Enabling interprocedural optimizations.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
else ()
    message("Interprocedural optimizations are not supported.")
endif ()
```

CMake targets are generated for install, allowing the project to be consumed by other CMake projects:

```cmake
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/Sample"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")
```

This allows another project that wants to depend on this one to configure itself automatically, like so:

```cmake
find_package(CommonLibSSESamplePlugin CONFIG REQUIRED)
# ...
target_link_libraries(${PROJECT_NAME} PRIVATE CommonLibSSESamplePlugin)
```

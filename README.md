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


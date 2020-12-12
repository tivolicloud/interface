# OS specific guides (recommended)

-   [BUILD_WINDOWS.md](BUILD_WINDOWS.md) - complete instructions for Windows
-   [BUILD_MACOS.md](BUILD_MACOS.md) - additional instructions for macOS
-   [BUILD_LINUX.md](BUILD_LINUX.md) - additional instructions for Linux
-   [BUILD_ANDROID.md](BUILD_ANDROID.md) _...may not work anymore_

We also recommended learning from example when new to building Tivoli. There are many complex systems in place and it's not always straightforward and easy.

You can find our official production pipelines for GitLab here:
https://git.tivolicloud.com/tivolicloud/interface/-/tree/main/.gitlab

# System dependencies

-   [git](https://git-scm.com/downloads)
-   [cmake](https://cmake.org/download)
-   [Python](https://www.python.org/downloads)
-   [Node.js](https://nodejs.org/en) v10.13 or higher

# CMake

Tivoli uses CMake to generate build files and project files for your platform. If you don't know how to use CMake, you may struggle to understand what's going on.

## Qt

When CMake is preparing build files, it will download our version of Qt.

Usually this is stored in your home directory as `tivoli/qt`.

As this folder can get quite big, you change the directory of where Qt will be downloaded by setting the environment variable (preferably system-wide) `TIVOLI_QT_BASE`.

```bash
    # windows
    set TIVOLI_QT_BASE=C:\TIVOLI_QT
    # unix
    export TIVOLI_QT_BASE=/home/user/TIVOLI_QT
```

Whilst this is not recommended with regular Qt installs, if you want to change the installation of Qt, you will need to set the environment variable `QT_CMAKE_PREFIX_PATH` pointing to your Qt **lib/cmake** folder.

```bash
    # windows
    set QT_CMAKE_PREFIX_PATH=C:\path\to\qt\lib\cmake
    # unix
    export QT_CMAKE_PREFIX_PATH=/path/to/qt/lib/cmake
```

## Vcpkg

When CMake is preparing build files, it will download and bootstrap Vcpkg and use it to install packages.

Usually this is stored in your home directory as `tivoli/vcpkg`.

Building all of our dependencies can be lengthy and take up a lot of disk space. You can change the directory using the environment variable `TIVOLI_VCPKG_BASE`

```bash
    # windows
    set TIVOLI_VCPKG_BASE=C:\TIVOLI_VCPKG
    # unix
    export TIVOLI_VCPKG_BASE=/home/user/TIVOLI_VCPKG
```

## Generating build files

Create a build directory in the root of your checkout and then run the CMake build from there. This will keep the rest of the directory clean.

```bash
    mkdir build
    cd build
    cmake ..
```

If cmake gives you the same error message repeatedly after the build fails, try removing `CMakeCache.txt` or the entire build folder.

## Variables

Any variables that need to be set for CMake can be set as environment variables or passed directly with a `-D` flag appended to the `cmake ..` command.

For example, to pass the `QT_CMAKE_PREFIX_PATH` variable (if not using our version of Qt) during build file generation without using an environment variable:

```bash
    cmake .. -DQT_CMAKE_PREFIX_PATH=/path/to/qt/lib/cmake
```

## Load QML from resources folder, not `resources.rcc`

When working on QML, you'll need a way to easily see the changes you're making.

-   Make sure you're making a **dev build** (`cmake ..` without any production variables). You can verify this in `build/includes/BuildInfo.h` with `VERSION = "dev"`.

-   When launching interface, add `HIFI_USE_SOURCE_TREE_RESOURCES=1` environment variable. You can add this in the launcher under **Developer > Additional environment variables**.

-   Once in-world, you can test your QML and press **Edit > Clear Cache & Reload (very slow)** to reload.

## Other runtime environment variables

-   `HIFI_SHOW_DEVELOPER_CRASH_MENU` - enables menu to test crashing
-   `TIVOLI_SCRIPT_DEBUG` - adds "Script Debug" menu for all running scripts

## Optional components

The following build options can be used when running CMake

### Build options

-   `BUILD_CLIENT`
-   `BUILD_SERVER`
-   `BUILD_TESTS`
-   `BUILD_TOOLS`

### Developer build options

-   `USE_GLES`
-   `DISABLE_UI`

## Running a domain server locally for testing

We recommend using the `pm2` process manager

-   Install Node.js
-   `npm i -g pm2`

Enter the root directory of this repo and run:

```bash
pm2 start ecosystem.config.js
# here are extra commands
pm2 ls
pm2 logs 0 # id of the process found in ls
pm2 logs 0 --lines 999
pm2 stop 0
pm2 start 0
pm2 restart 0
pm2 kill
```

**Don't forget to run `pm2 kill`** when you're finished or it'll keep running in the background!

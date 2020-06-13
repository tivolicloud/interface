# OS specific guides (recommended)

-   [BUILD_WIN.md](BUILD_WIN.md)
-   [BUILD_OSX.md](BUILD_OSX.md)
-   [BUILD_LINUX.md](BUILD_LINUX.md)
-   [BUILD_ANDROID.md](BUILD_ANDROID.md) _...may not work anymore_

We also recommended learning from example when new to building Tivoli. There are many complex systems in place and it's not always straightforward and easy.

You can find our official production pipelines for GitLab here:
https://git.tivolicloud.com/tivolicloud/interface/-/tree/master/.gitlab

# System dependencies

-   [git](https://git-scm.com/downloads)
-   [cmake](https://cmake.org/download)
-   [Python](https://www.python.org/downloads)
-   [Node.JS](https://nodejs.org/en)

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

Any variables that need to be set for CMake can be set as environement variables or passed directly with a `-D` flag appended to the `cmake ..` command.

For example, to pass the `QT_CMAKE_PREFIX_PATH` variable (if not using our version of Qt) during build file generation without using an environment variable:

```bash
    cmake .. -DQT_CMAKE_PREFIX_PATH=/path/to/qt/lib/cmake
```

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

# Building Tivoli for Windows

## Step 1. Installing dependencies

Download and install these programs:

-   **Visual Studio 2019**: https://visualstudio.microsoft.com/vs

    When selecting components, make sure to check **Desktop development with C++**.

-   **CMake 3.15 or higher**: https://cmake.org/download

    During installation, make sure to check **Add CMake to system PATH for all users** when prompted.

-   **Python 3 or higher**: https://www.python.org/downloads

-   **Node.js 12 or higher**: https://nodejs.org/en

Make sure `cmake`, `python` and `node` are available in your PATH.

## Step 2. Configuring environment variables

You can set these as **system environment variables** or write a **helper batch file**.

-   **For Vcpkg**

    CMake will **automatically download Vcpkg** to compile required dependencies for compiling Tivoli.

    It will use the directory: `C:\Users\[username]\hifi\vcpkg`

    This folder **will get big!** It's possible to change it by setting a variable:

    `set HIFI_VCPKG_BASE=D:\path\to\vcpkg`

-   **For Qt**

    CMake will also **download a custom version of Qt** which is required.

    It will use the directory: `C:\Users\[username]\hifi\qt`

    It will **also get big!** Use this environement variable to change it:

    `set HIFI_QT_BASE=D:\path\to\qt`

-   **Developer or production build**

    By default, it will create a developer build.

    Set these environment variables for a production build:

    `set RELEASE_TYPE=PRODUCTION`

    `set STABLE_BUILD=1`

    `set RELEASE_NUMBER=1.2.3` which is unnecessary

    Please do not set these as system variables.

## Step 3. Cloning and preparing

Open the command prompt and git clone interface

```cmd
git clone https://git.tivolicloud.com/tivolicloud/interface
cd interface
```

You can use the master branch (default) or checkout to the latest tag

```
git tag
git checkout tags/0.8.2
```

Once you're checked out on the right version

```cmd
mkdir build
cd build

cmake .. -G "Visual Studio 16 2019" -A x64
```

CMake will now download dependencies including Qt and prepare build files.

Please wait. It will take a while... It really will!

## Step 4. Making a Build

-   **Using Visual Studio 2019**

    Open `interface\build\hifi.sln`.

    Change the **Solution Configuration** (next to the green play button at the top) from **Debug** to **RelWithDebInfo** for best performance and debugging capabilities.

    On the right sidebar in the **Solution Explorer**, right mouse click **interface** and click **Build**

-   **Using the command prompt**

    Open the command prompt

    ```cmd
    cd C:\path\to\tivoli\interface\build

    cmake --build . --target interface --config RelWithDebInfo
    ```

## Step 5. Running interface

You can run interface using the launcher: https://alpha.tivolicloud.com/download

In the launcher under **Settings**, enable **Developer settings**. Then in the new menu, set **Interface dir** to `C:\path\to\tivoli\interface\build\interface\RelWithDebInfo` which should contain `interface.exe`

If you want to run Tivoli without the launcher, run:

```bash
interface.exe --tokens [current access token]
```

You can find your access token in the launcher's developer menu. Please don't share it and keep it safe!

When debugging, you'll likely have to open Tivoli without the launcher.

## Troubleshooting

If your build fails, you could ask around on [our Discord](https://alpha.tivolicloud.com/discord) for help.

Deleting the `build` folder and trying again may help.

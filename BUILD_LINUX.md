# Building Tivoli for Linux

# WARNING! NOT ALL DISTROS ARE SUPPORTED!

Tivoli uses a custom version of Qt that needs to be compiled for a list of distros that we choose.

Currently we support building on:

-   Ubuntu 18.04 _(production builds use this)_
-   Ubuntu 18.04 arm64v8 _(production builds use this)_
-   Ubuntu 20.04
-   Arch Linux

We're looking into ways to make this easier. Compiling Qt yourself takes a very long time, therefore it's not viable to put it in our build pipeline.

## Step 1. Installing dependencies

Make sure you have general build tools, `git`, `cmake`, `python` 3+ and `node` 12+ installed.

We'll update this with the proper package lists for supported distros soon.

A verbose list of packages used in our build system can be found here: https://git.tivolicloud.com/tivolicloud/interface/-/blob/master/.gitlab/build/linux.gitlab-ci.yml

## Step 2. Configuring environment variables

You can set these however you like. We recommend you write a **helper shell script** but that's not necessary.

-   **For Vcpkg**

    CMake will **automatically download Vcpkg** to compile required dependencies for compiling Tivoli.

    It will use the directory: `~/tivoli/vcpkg`

    This folder **will get big!** It's possible to change it by setting a variable:

    `export TIVOLI_VCPKG_BASE=~/path/to/vcpkg`

-   **For Qt**

    CMake will also **download a custom version of Qt** which is required.

    It will use the directory: `~/tivoli/qt`

    It will **also get big!** Use this environment variable to change it:

    `export TIVOLI_QT_BASE=~/path/to/qt`

-   **Developer or production build**

    By default, it will create a developer build.

    Set these environment variables for a production build:

    `export RELEASE_TYPE=PRODUCTION`

    `export STABLE_BUILD=1`

    `export RELEASE_NUMBER=1.2.3` which is unnecessary

## Step 3. Cloning and preparing

Open the terminal and git clone interface

```bash
git clone https://git.tivolicloud.com/tivolicloud/interface
cd interface
```

You can use the master branch (default) or checkout to the latest tag

```
git tag
git checkout tags/???
```

Once you're checked out on the right version

```bash
mkdir build
cd build

cmake -G "Unix Makefiles" ..
```

CMake will now download dependencies including Qt and prepare build files.

Please wait. It will take a while... It really will!

## Step 4. Making a Build

Find the amount of **physical CPU cores** your computer has.

If you have an 8 cores desktop Intel CPU, you're likely to have 4 physical cores and 4 hyper threaded. Hyper threaded cores do not count.

Open the terminal

```bash
cd /path/to/interface/build

cmake --build . --target interface --config RelWithDebInfo
```

Some available targets are: `interface`, `domain-server`, `assignment-client`

## Step 5. Running interface

You can run interface using the launcher: https://tivolicloud.com/download

In the launcher under **Settings**, enable **Developer settings**. Then in the new menu, set **Interface dir** to `/path/to/interface/build/interface` which should contain `interface` executable

If you want to run Tivoli without the launcher, run:

```bash
interface --tokens [current access token]
```

You can find your access token in the launcher's developer menu. Please don't share it and keep it safe!

When debugging, you'll likely have to open Tivoli without the launcher.

## Troubleshooting

If your build fails, you could ask around on [our Discord](https://tivolicloud.com/discord) for help.

Deleting the `build` folder and trying again may help.

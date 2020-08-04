# Building Tivoli for macOS

## Step 1. Installing dependencies

Download and install these programs:

-   **Xcode**

    You can find Xcode on the **App Store**.

-   **MacOSX 10.11 SDK**

    You'll have to download the **10.11 SDK** because compiling with newer SDKs like 10.15 (Catalina) causes lots of issues.

    To install, make sure Xcode is installed and follow these instructions in your terminal:

    ```bash
    cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs

    sudo curl -LJO https://github.com/phracker/MacOSX-SDKs/releases/download/10.15/MacOSX10.11.sdk.tar.xz

    sudo tar -xvf MacOSX10.11.sdk.tar.xz
    sudo rm -f MacOSX10.11.sdk.tar.xz
    ```

-   **Homebrew package manager**: https://brew.sh

    Once installed, open the terminal and run:

    The last command will install OpenSSL 1.0.2t

    ```bash
    brew install cmake python
    brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/64555220bfbf4a25598523c2e4d3a232560eaad7/Formula/openssl.rb -f
    ```

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

cmake -DOPENSSL_ROOT_DIR=/usr/local/Cellar/openssl/1.0.2t -DOPENSSL_INCLUDE_DIR=/usr/local/Cellar/openssl/1.0.2t/include -DOPENSSL_USE_STATIC_LIBS=TRUE -DOSX_SDK=10.11 -G Xcode ..
```

**Note:** OpenSSL will soon be a Vcpkg dependency. You won't have to install it and it will make the prepare command smaller.

CMake will now download dependencies including Qt and prepare build files.

Please wait. It will take a while... It really will!

## Step 4. Making a Build

-   **Using Xcode**

    Open `interface\build\hifi.xcodeproj`.

    It will ask if you want to **Autocreate Schemes**. Click yes.

    Next to the play and stop button (top left), you'll find a **target icon with name next to it**. Click that and type/select: **interface**.

    Click the **target icon with name next to it** again and you'll find at the bottom: **Edit Scheme**.

    In the scheme config popup, set the **Build Configuration** from **Debug** to **RelWithDebInfo** and click close.

    Click the **play button** and it will start building.

-   **Using the terminal**

    Open the terminal

    ```bash
    cd /path/to/tivoli/interface/build

    cmake --build . --target interface --config RelWithDebInfo
    ```

    Some available targets are: `interface`, `domain-server`, `assignment-client`

## Step 5. Running interface

You can run interface using the launcher: https://tivolicloud.com/download

In the launcher under **Settings**, enable **Developer settings**. Then in the new menu, set **Interface dir** to `/path/to/interface/build/interface/RelWithDebInfo` which should contain `interface` executable

If you want to run Tivoli without the launcher, run:

```bash
interface --tokens [current access token]
```

You can find your access token in the launcher's developer menu. Please don't share it and keep it safe!

When debugging, you'll likely have to open Tivoli without the launcher.

## Troubleshooting

If your build fails, you could ask around on [our Discord](https://tivolicloud.com/discord) for help.

Deleting the `build` folder and trying again may help.

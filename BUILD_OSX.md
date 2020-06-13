Please read the [general build guide](BUILD.md) for information on dependencies required for all platforms. Only macOS specific instructions are found in this file.

### Homebrew

[Homebrew](https://brew.sh/) is an excellent package manager for macOS. It makes install of some Tivoli Cloud VR dependencies very simple.

```bash
brew install cmake openssl
```

### Python 3

Download an install Python 3.6.6 or higher from [here](https://www.python.org/downloads/).  
Execute the `Update Shell Profile.command` script that is provided with the installer.

### OpenSSL

Assuming you've installed OpenSSL using the homebrew instructions above, you'll need to set OPENSSL_ROOT_DIR so CMake can find your installations.  
For OpenSSL installed via homebrew, set:

```bash
export OPENSSL_ROOT_DIR=/usr/local/Cellar/openssl/1.0.2t
export OPENSSL_INCLUDE_DIR=${OPENSSL_ROOT_DIR}/include
```

Note that this uses the version from the homebrew formula at the time of this writing, and the version in the path will likely change.

### MacOSX 10.11 SDK

You will need to download the 10.11 SDK. Compiling with 10.15 (Catalina) causes lots of issues.

```bash
cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs
sudo curl -LJO https://github.com/phracker/MacOSX-SDKs/releases/download/10.15/MacOSX10.11.sdk.tar.xz
sudo tar -xvf MacOSX10.11.sdk.tar.xz
sudo rm -f MacOSX10.11.sdk.tar.xz
```

Make sure to set the enviroment variable:

```bash
export OSX_SDK=10.11
```

### Xcode

If Xcode is your editor of choice, you can ask CMake to generate Xcode project files instead of Unix Makefiles.

```bash
cmake .. -G Xcode
```

If `cmake` complains about Python 3 being missing, you may need to update your CMake binary with command `brew upgrade cmake`, or by downloading and running the latest CMake installer, depending on how you originally instaled CMake

After running cmake, you will have the make files or Xcode project file necessary to build all of the components. Open the hifi.xcodeproj file, choose ALL_BUILD from the Product > Scheme menu (or target drop down), and click Run.

If the build completes successfully, you will have built targets for all components located in the `build/${target_name}/Debug` directories.

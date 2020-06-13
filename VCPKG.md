[Vcpkg](https://github.com/Microsoft/vcpkg) is an open source package management system created by Microsoft, intially just for Windows based system, but eventually extended to cover Linux and OSX as well, and in theory extensible enough to cover additional operating systems.

Vcpkg is now our primary mechanism for managing the external libraries and tools on which we rely to build our applications.

Conventional usage of Vcpkg involves cloning the repository, running the bootstrapping script to build the vcpkg binary, and then calling the binary to install a set of libraries. The libraries themselves are specified by a set of port files inside the [repository](https://github.com/Microsoft/vcpkg/tree/master/ports)

Because the main Vcpkg repository does not contain all the ports we want, we use a set of [custom port files](./cmake/ports) stored in our own repository.

# Adding new packages to vcpkg

Note... Android vcpkg usage is still experimental.

## Setup development environment

In order to add new packages, you will need to set up an environment for testing. This assumes you already have the tools for normal Hifi development (git, cmake, a working C++ compiler, etc)

-   Just clone this repo and prepare. Through the logs, you'll find the bootstrapped vcpkg directory.

## Add a new port

Resort to [the official vcpkg repo](https://github.com/Microsoft/vcpkg) for examples and [the vcpkg docs](https://vcpkg.readthedocs.io/en/latest) for additional help to add a new port to our `cmake/ports` folder.

## Add a reference to your package to our meta-packages

We have three meta-packages used to do our building.

<!-- When you modify one of these packages, make sure to bump the version number in the `CONTROL` file for the package -->

### hifi-deps

This meta-package contains anything required for building the server or shared components. For instance, the `glm`, `tbb` and `zlib` packages are declared here because they're used everywhere, not just in our client application code.

### hifi-client-deps

This meta-package contains anything required for building the client. For example, `sdl2` is listed here because it's required for our joystick input, but not for the server or shared components. Note that `hifi-client-deps` depends on `hifi-deps`, so you don't have to declare something twice if it's used in both he server and client. Just declare it in `hifi-deps` and it will still be includeded transitively.

### hifi-host-tools

This meta-package contains anything we use to create executables that will then be used in the build process. The `hifi-deps` and `hifi-client-deps` packages are built for the target architecture, which may be different than the host architecture (for instance, when building for Android). The `hifi-host-tools` packages are always build for the host architecture, because they're tools that are intended to be run as part of the build process. Scribe for example is used at build time to generate shaders. Building an arm64 version of Scribe is useless because we need to run it on the host machine.

Note that packages can appear in both the `hifi-host-tools` and one of the other meta-packages, indicating that the package both contains a library which we will use at runtime, and a tool which we will use at build time. The `spirv-tools` package is an example.

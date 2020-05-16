# Tivoli Cloud VR

Tivoli Cloud VR (Tivoli) is a massive online social platform for virtual reality and desktop.

This repository contains all components needed to build our interface as well as servers.
You will find compiling support for several operating systems and architectures.

## Documentation

Documentation is available at [docs.tivolicloud.com](https://docs.tivolicloud.com), if something is missing, please suggest it on our [git issues](https://git.tivolicloud.com/tivolicloud/issues/-/issues) or [roadmap](https://roadmap.tivolicloud.com).

The JavaScript API reference can be found under [https://apidocs.tivolicloud.com](https://apidocs.tivolicloud.com).

There is also detailed [documentation on our coding standards](howtos/CODING_STANDARD.md).

## License

Tivolis is [AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html)

Built atop HFVRP Copyright 2019 High Fidelity, Inc.

## Building

All information required to build is found in the [build guide](howtos/BUILD.md).

## Running the interface

You can run the interface using the launcher available at: https://alpha.tivolicloud.com/download

If you're using a self-compiled version, you can set the build location in the launcher under the developer menu.

If you want to run Tivoli without the launcher, run:
```
interface.exe --tokens [current access token]
```
You can find your access token in the launcher's developer menu. Please don't share it and keep it safe! 

## Running your own server 

The server is available on Docker hub: https://hub.docker.com/r/tivolicloud/server

If you want to run a self-compiled version, refer to: https://git.tivolicloud.com/tivolicloud/interface/-/tree/master/docker/server

A more comprehensive guide will be written soon.

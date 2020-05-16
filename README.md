# Tivoli Cloud Vr
Tivoli Cloud Vr (Tivoli) is a massive online social platform for virtual reality and desktop.

This repository contains all components needed to build our interface as well as servers.
You will find compiling support for several operating systems and architectures.

## Documentation
Documentation is available at [docs.tivolicloud.com](https://docs.tivolicloud.com), if something is missing, please suggest it via a new item on our [gitlab](https://git.tivolicloud.com/tivolicloud/issues/-/issues) or [roadmap](https://roadmap.tivolicloud.com/).

The JavaScript Documentation can be found under [apidocs.tivolicloud.com](https://apidocs.tivolicloud.com/).

There is also detailed [documentation on our coding standards](howtos/CODING_STANDARD.md).

## License
Built atop HFVRP copyright 2019 High Fidelity, Inc. under Apache 2.0
See http://www.apache.org/licenses/LICENSE-2.0.html and [license](tivoli-agpl3-header.txt)

## Building
All information required to build is found in the [build guide](howtos/BUILD.md).

## Running the interface
You can run the the interface with the following:
```
interface.exe --tokens [current access token]
```
The current access token is only needed, if you want to log in with your Tivoli Account.
You can find it under the developer options in the launcher.

You can also point the interface path to your custom build location there. Then you can use the launcher to start the interface with your account.

~~Running your own servers~~
========
~~The assignment-client and domain-server are architectural components that will allow~~
~~you to run the full stack of the virtual world.~~

~~In order to set up your own virtual world, you need to set up and run your own~~
~~local "domain".~~

~~The domain-server gives a number different types of assignments to the assignment-client~~
~~for different features: audio, avatars, voxels, particles, meta-voxels and models.~~

~~Follow the instructions in the [build guide](BUILD.md) to build the various components.~~

~~From the domain-server build directory, launch a domain-server.~~

    ./domain-server

~~Then, run an assignment-client. The assignment-client uses localhost as its assignment-server~~
~~and talks to it on port 40102 (the default domain-server port).~~

~~In a new Terminal window, run:~~

    ./assignment-client

~~Any target can be terminated with Ctrl-C (SIGINT) in the associated Terminal window.~~

~~This assignment-client will grab one assignment from the domain-server. You can tell the~~
~~assignment-client what type you want it to be with the `-t` option. You can also run an~~
~~assignment-client that forks off *n* assignment-clients with the `-n` option. The `-min`~~
~~and `-max` options allow you to set a range of required assignment-clients. This allows~~
~~you to have flexibility in the number of assignment-clients that are running.~~
~~See `--help` for more options.~~

    ./assignment-client --min 6 --max 20

~~To test things out, you'll need to run the Interface client.~~

~~To access your local domain in Interface, open your Preferences. On OS X, this is available~~
~~in the Interface menu. On Linux, you'll find it in the File menu. Enter "localhost" in the~~
~~"Domain server" field.~~

~~If everything worked, you should see that you are connected to at least one server.~~
~~Nice work!~~
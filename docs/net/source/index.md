<p align="center"><img src="images/logo-header.svg"></p>

# RobotRaconteurNET Documentation

Welcome to the RobotRaconteurNET documentation!

Robot Raconteur is a communication framework for robotics, automation, and the Internet of Things. Robot Raconteur Core,
the primary library supporting Robot Raconteur, supports many programming languages including C++, Python, Java,
Matlab, C\#, and LabVIEW. `RobotRaconteurNET` provides support for .NET languages including C\#, VB.net, and F\#.

**See the API Documentation here!!** [api/index.html](api/index.html)

## Robot Raconteur

Robot Raconteur as framework and ecosystem is documented on the Robot Raconteur product page and GitHub homepage.
The tutorials are targeted at Python and C++, however the concepts are identical regardless of the programming
language interface used. It is recommended the user familiarizes themselves with the framework using Python
before using C\#.

Homepage: http://robotraconteur.com
GitHub Page: https://github.com/robotraconteur/robotraconteur
Documentation Listing: https://github.com/robotraconteur/robotraconteur/wiki/Documentation

## Installation
### .NET Installation

`RobotRaconteurNET` can be easily installed using the NuGet package:

NuGet package homepage: https://www.nuget.org/packages/RobotRaconteurNET/

From the command line:

```
dotnet add package RobotRaconteurNET
```

`RobotRaconteurNET` can also be added to a project using the NuGet browser built in to Visual Studio.

The NuGet package includes the native library for Windows x86 and x64

### Linux and MacOS Support

`RobotRaconteurNET` can also be used on Linux and MacOS using .NET Core or Mono, but the NuGet package does
not include the native library for these platforms.

For Debian and Ubuntu, the native library can be installed using `apt`, after being configured for the Robot Raconteur
repository. See https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md for
instructions to set up the `apt` repository.

After the `apt` repository is configured, run:

```
sudo apt-get install librobotraconteur-net-native
```

MacOS does not currently have a binary package available. Use of the Conda package is recommended.

The Linux Conda package is also available and includes the native library.

### Conda Packages

Conda packages are available from the `robotraconteur` channel. Install using:

```
conda install -c conda-forge -c robotraconteur robotraconteur
```

The `robotraconteur` package includes the native library, but does not include the managed library. Use NuGet with
your project for the managed library.

## Single RobotRaconteurNode support

The `RobotRaconteurNET` library only supports one Robot Raconteur node per process.

Copyright (C) 2023 Wason Technology, LLC

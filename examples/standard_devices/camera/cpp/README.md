# Camera C++ Client Examples

This directory contains examples for capturing and displaying images from standard cameras.
Use with either the [robotraconteur_training_sim](https://github.com/robotraconteur-contrib/robotraconteur_training_sim)
or with a Robot Raconteur driver that exposes the standard camera service type such as the
[robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver).
By default the examples will connect to the URL for the training
simulator simulated camera. Pass the URL of the real camera driver if using a real camera.
The real URL can be found using the
[Robot Raconteur Service Browser](https://github.com/robotraconteur/RobotRaconteur_ServiceBrowser).

Examples:

- `camera_client_capture_frame.cpp` - Capture and display a single image frame

## Setup

See the [Simple Webcam client example readme](../../../simple_webcam/cpp/client/README.md)
for instructions to set up the C++ environment.

These examples require the `robotraconteur-companion` package in addition to the packages used in the Simple Webcam
example. Run the following to install:

Windows:

```
vcpkg install --triplet=x64-windows robotraconteur-companion
```

Ubuntu:

```
sudo apt-get install librobotraconteur-companion-dev
```

## Compiling

### Windows

```
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cmake -GNinja -S. -B build
cmake --build build
```

## Run Examples

Example executables are in the `build/` directory.

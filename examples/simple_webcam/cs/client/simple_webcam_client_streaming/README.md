# Simple Webcam Client C\# Robot Raconteur Example

This example demonstrates a simple webcam client using Robot Raconteur and C\#.

Connects to the single camera service and streams images
from the camera using the `frame_stream` pipe member.
The images are displayed for the user. Press escape to close the window and exit the program.

The examples assume that the corresponding `simple_webcam_service` service is running. Any of the simple webcam service
examples should work regardless of language. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Setup

These setup steps only need to be run once, however other examples may require additional packages to be installed.
Check the instructions for the example for additional setup steps.

Install the "dotnet" SDK from https://dotnet.microsoft.com/download. In this example, dotnet 6 is used.

Platforms other than Windows require the native library to be installed. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md). For
Ubuntu, the package `librobotraconteur-net-native` package must be installed.

## Run Example

Navigate to the `examples/simple_webcam/cs/service/simple_webcam_service` directory in a terminal and run:

```bash
dotnet run --framework net6.0
```

Open a new terminal and navigate to the `examples/simple_webcam/cs/client/simple_webcam_client_streaming` directory and run:

```bash
dotnet run --framework net6.0
```

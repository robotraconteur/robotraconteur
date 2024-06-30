# Simple Webcam Service C\# Robot Raconteur Example

This example demonstrates a simple Robot Raconteur service to access a single webcam using C\# and OpenCV.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The single and multi camera services by default listen on TCP port 22355. Note that only
one of the services can be used at a time because they listen on the same TCP port. Use the following connection
information to connect to the services:

- Single Camera Service
  - URL: `rr+tcp://localhost:22355?service=webcam`
  - Node Name: `experimental.simplewebcam3`
  - Service Name: `webcam`
  - Root Object Type: `experimental.simplewebcam3.Webcam`

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

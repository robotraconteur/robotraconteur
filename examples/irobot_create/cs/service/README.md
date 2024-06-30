# iRobot Create C\# Robot Raconteur Service Example

The iRobot Create service provides a simple example of a Robot Raconteur service. The service uses the
serial port OI interface to communicate with the iRobot Create. The service allows clients to drive the robot,
control the LEDs, and receive state feedback information.

The iRobot Create example demonstrates using `property`, `function`, `event`, `callback`, and `wire` members.
It also demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The iRobot Create service by defaults listens on port 22354. Use the following connection information
to connect to the service:

- URL: `rr+tcp://localhost:22354?service=create`
- Node Name: `experimental.create3`
- Service Name: `create`
- Root Object Type: `experimental.create3.Create`

## Setup

These setup steps only need to be run once, however other examples may require additional packages to be installed.
Check the instructions for the example for additional setup steps.

Install the "dotnet" SDK from https://dotnet.microsoft.com/download. In this example, dotnet 6 is used.

Platforms other than Windows require the native library to be installed. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md). For
Ubuntu, the package `librobotraconteur-net-native` package must be installed.

## Run Example

Navigate to the `examples/irobot_create/cs/service/` directory in a terminal and run:

```bash
dotnet run --framework net6.0 -- COM4
```

Replace `COM4` with the serial port of the iRobot Create. On Linux the serial port may be `/dev/ttyUSB0`.

Open a new terminal and navigate to the `examples/irobot_create/cs/client` directory and run:

```bash
dotnet run --framework net6.0
```

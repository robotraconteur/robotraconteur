# iRobot Create C\# Robot Raconteur Client Examples

The iRobot Create Client example demonstrates how to use the iRobot Create service with Robot Raconteur.
The client drives the robot for a short distance, reads the states, and provides a callback for playing
a short song. (In most cases a callback would not be used for this function, but it is a good example of how to use callbacks.)

Press the "Play" button on the Create to play the song. The callback will be called when the song is played.

This example assumes that the iRobot Create Service is running. Any implementation of the service from the examples
can be used. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The iRobot Create example demonstrates using `property`, `function`, `event`, `callback`, and `wire` members.
It also demonstrates using constants and structures.

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

Navigate to the `examples/irobot_create/cs/service/` directory in a terminal and run:

```bash
dotnet run --framework net6.0 -- COM4
```

Open a new terminal and navigate to the `examples/irobot_create/cs/client` directory and run:

```bash
dotnet run --framework net6.0
```

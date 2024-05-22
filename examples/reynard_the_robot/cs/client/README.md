# Reynard C\# Robot Raconteur Client Example

This example demonstrates controlling Reynard using C\# as a Robot Raconteur client.

dotnet core 5.0 SDK or greater is required. See https://dotnet.microsoft.com/en-us/download . In this example,
dotnet 6 is used.

Platforms other than Windows require the native library to be installed. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md). For
Ubuntu, the package `librobotraconteur-net-native` package must be installed.

## Run Example

Start the Reynard the Robot server by following the instructions in the [Python example Readme](../../python/client/README.md).

The `dotnet run` command will install the RobotRaconteurNET library and compile the example.

```
dotnet run --framework net6.0
```

Replace `net6.0` with the appropriate dotnet version.

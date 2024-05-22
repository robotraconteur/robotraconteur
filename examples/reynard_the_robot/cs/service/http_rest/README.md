# Reynard C\# Robot Raconteur Service Example HTTP REST Backend

This example demonstrates a Robot Raconteur service to control Reynard using C\# with HTTP REST communication
with the robot. This example is intended to be representative of the types of drivers that are used with real
industrial robots and devices.

## Setup

These setup steps only need to be run once, however other examples may require additional packages to be installed.
Check the instructions for the example for additional setup steps.

Install the "dotnet" SDK from https://dotnet.microsoft.com/download. In this example, dotnet 6 is used.

Platforms other than Windows require the native library to be installed. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md). For
Ubuntu, the package `librobotraconteur-net-native` package must be installed.

The `reynard-the-robot` Python package must be installed. See the [Python example README](../../python/service/README.md)
for instructions.

## Run Example

Start the Reynard the Robot server:

```cmd
python -m reynard_the_robot --disable-robotraconteur
```

On Linux, `python3` may need to be used instead of `python`.

Open a second command prompt and navigate to the `examples/reynard_the_robot/cs/service/http_rest` directory.
Run the following command:

```cmd
dotnet run --framework net6.0
```

The example Robot Raconteur service listens on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```

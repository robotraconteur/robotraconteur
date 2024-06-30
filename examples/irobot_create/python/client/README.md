# iRobot Create Python Robot Raconteur Client Examples

The iRobot Create Client example `irobot_create_client.py` demonstrates how to use the iRobot Create service
with Robot Raconteur.
The client drives the robot for a short distance, reads the states, and provides a callback for playing
a short song. (In most cases a callback would not be used for this function, but it is a good example of how
to use callbacks.)
A second client example `irobot_create_client_joystick.py` demonstrates how to use a gamepad to drive the robot
using PyGame to read the gamepad inputs.

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

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur drekar-launch-process pygame
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur drekar-launch-process pygame
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and start the iRobot Create service. For the Python service run:

```cmd
cd robotraconteur\examples\irobot_create\python\service
python irobot_create_service.py COM4
```

Run the client example:

```cmd
cd robotraconteur\examples\irobot_create\python\client
python irobot_create_client.py
```

Use `irobot_create_client_joystick.py` to drive the robot with a gamepad:

```cmd
python irobot_create_client_joystick.py
```

### Ubuntu

Open a terminal and start the iRobot Create service. For the Python service run:

```bash
cd robotraconteur/examples/irobot_create/python/service
python3 irobot_create_service.py /dev/ttyUSB0
```

Run the client example:

```bash
cd robotraconteur/examples/irobot_create/python/client
python3 irobot_create_client.py
```

Use `irobot_create_client_joystick.py` to drive the robot with a gamepad:

```bash
python3 irobot_create_client_joystick.py
```

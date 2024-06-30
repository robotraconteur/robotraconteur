# iRobot Create Python Robot Raconteur Service Example

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

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur drekar-launch-process pyserial pygame
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur drekar-launch-process pyserial pygame
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and navigate to the `examples/irobot_create/python/service` directory.
Run the following command:

```cmd
python irobot_create_service.py COM4
```

The example services listen on port 22354. Run a client examples:

```cmd
cd robotraconteur\examples\irobot_create\python\client
python irobot_create_client.py
```

### Ubuntu

Open a terminal and navigate to the `examples/irobot_create/python/service` directory.
Run the following command:

```bash
python3 irobot_create_service.py /dev/ttyUSB0
```

The example services listen on port 22354. Run a client example:

```bash
cd robotraconteur/examples/irobot_create/python/client
python3 irobot_create_client.py
```

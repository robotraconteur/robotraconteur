# Reynard Python Robot Raconteur Client Example

This example demonstrates how to use Robot Raconteur to control Reynard the Robot using a Python client. The
example expects the Python Reynard the Robot server to be running. The following instructions explain how to set up
the requirements and run the example. This example uses the plug-and-play capabilities to connect to the Reynard
service and automatically generate the required type information.

Instructions are provided for Windows and Ubuntu. These examples will work on other platforms with minor modifications.

## Setup

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur reynard-the-robot
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur reynard-the-robot
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot
```

Open a second command prompt and navigate to the `examples/reynard_the_robot/python/client` directory.
Run the following command:

```cmd
python reynard_robotraconteur_client.py
```

### Ubuntu

Open a terminal and start the Reynard the Robot server:

```bash
python3 -m reynard_the_robot
```

Open a second terminal and navigate to the `examples/reynard_the_robot/python/client` directory. Run the following command:

```bash
python3 reynard_robotraconteur_client.py
```

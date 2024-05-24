# Reynard Python Robot Raconteur Service Examples

Examples are provided for creating Robot Raconteur services to control Reynard the Robot. These examples
communicate with Reynard using HTTP REST, ASCII Sockets, or the Python API. These examples are intended to be
representative of the types of drevires that are used with real industrial robots and devices.

The `reynard_robotraconteur_service_http_rest.py` example demonstrates a service that uses HTTP REST to communicate
with Reynard. The `reynard_robotraconteur_service_ascii_socket.py` example demonstrates a service that uses an ASCII
Socket. The `reynard_robotraconteur_service_api.py` example demonstrates a service that uses the Python API. With the
Python API, the Reynard instance runs in the same process as the service. For the other examples, the Reynard instance
runs in a separate process and must be started separately.

Instructions are provided for Windows and Ubuntu. These examples will work on other platforms with minor modifications.

## Setup

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur reynard-the-robot requests drekar-launch-process
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur reynard-the-robot requests drekar-launch-process
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot --disable-robotraconteur
```

Open a second command prompt and navigate to the `examples/reynard_the_robot/python/service` directory.
Run the following command:

```cmd
python reynard_robotraconteur_service_http_rest.py
```

Or

```cmd
python reynard_robotraconteur_service_ascii_socket.py
```

Alternatively, run the following command to use the Python API:

```cmd
python reynard_robotraconteur_service_api.py
```

Do not start the Reynard the Robot server when using the Python API.

The example services listen on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```

### Ubuntu

Open a command prompt and start the Reynard the Robot server:

```bash
python3 -m reynard_the_robot --disable-robotraconteur
```

Open a second command prompt and navigate to the `examples/reynard_the_robot/python/service` directory.
Run the following command:

```bash
python3 reynard_robotraconteur_service_http_rest.py
```

Or

```bash
python3 reynard_robotraconteur_service_ascii_socket.py
```

Alternatively, run the following command to use the Python API:

```bash
python3 reynard_robotraconteur_service_api.py
```

Do not start the Reynard the Robot server when using the Python API.

The example services listen on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```

# Reynard C++ Robot Raconteur Service Examples

Examples are provided for creating Robot Raconteur services to control Reynard the Robot. These examples
communicate with Reynard using HTTP REST or ASCII Socket communication. These examples are intended to be
representative of the types of drivers that are used with real industrial robots and devices.

The `reynard_robotraconteur_service_http_rest` example demonstrates a service that uses HTTP REST to communicate
with Reynard, while the `reynard_robotraconteur_service_ascii_socket` example demonstrates a service that
uses an ASCII Socket.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Setup

These setup steps only need to be run once, however other examples may require additional packages to be installed.
Check the instructions for the example for additional setup steps.

### Windows

These instructions assume that a version of Visual Studio C++ is installed on the system. The
"Desktop Development with C++" workload must be installed in Visual Studio C++ to build the example.
CMake, Ninja, Git, and Python are also required. The easiest way to install all dependencies is to use Chocolatey.
See https://docs.chocolatey.org/en-us/choco/setup for instructions on installing Chocolatey.

Once Chocolatey, run the following in an elevated command prompt to install the required dependencies:

```cmd
choco install cmake ninja python git visualstudio2022community visualstudio2022-workload-nativedesktop
```

A version of Python must be installed on the system to run Reynard the Robot. Run the following command in a
command prompt to install the required Python packages:

```cmd
python -m pip install --user reynard-the-robot
```

Next, clone Robot Raconteur and vcpkg repositories:

```cmd
git clone https://github.com/robotraconteur/robotraconteur.git
git clone https://github.com/microsoft/vcpkg.git
```

Vcpkg must be bootstrapped and Robot Raconteur installed. Robot Raconteur is available as a vcpkg package in the main
repository. Run the following commands in a command prompt:

```cmd
cd vcpkg
bootstrap-vcpkg
vcpkg --triplet=x64-windows install robotraconteur boost-beast nlohmann-json drekar-launch-process-cpp
cd ..
```

### Ubuntu

The Robot Raconteur PPA must be configured to install Robot Raconteur. On Debian, the PPA is not available
and the [Robot Raconteur APT repository](https://github.com/robotraconteur/robotraconteur-apt) must be used.

bash```
sudo add-apt-repository ppa:robotraconteur/ppa
```

Install Robot Raconteur and the other dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake git ninja-build python3 python3-pip robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev libdrekar-launch-process-cpp-dev nlohmann-json3-dev
```

Install Reynard the Robot:

```bash
python3 -m pip install --user reynard-the-robot
```

Clone Robot Raconteur repository:

```cmd
git clone https://github.com/robotraconteur/robotraconteur.git
```

## Compiling

### Windows

```
cd robotraconteur\examples\reynard_the_robot\cpp\service
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cd robotraconteur/examples/reynard_the_robot/cpp/service
cmake -GNinja -S. -B build
cmake --build build
```

## Run Example

### Windows

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot --disable-robotraconteur
```

In another command prompt and run the service:

```cmd
cd robotraconteur\examples\reynard_the_robot\cpp\service\build
reynard_robotraconteur_service_http_rest.exe
```

Or

```cmd
cd robotraconteur\examples\reynard_the_robot\cpp\service\build
reynard_robotraconteur_service_ascii_socket.exe
```

The example services listen on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```

### Ubuntu

Open a terminal and start the Reynard the Robot server:

```bash
python3 -m reynard_the_robot --disable-robotraconteur
```

In another terminal and run the service:

```bash
cd robotraconteur/examples/reynard_the_robot/cpp/service/build
./reynard_robotraconteur_service_http_rest
```

Or

```bash
cd robotraconteur/examples/reynard_the_robot/cpp/service/build
./reynard_robotraconteur_service_ascii_socket
```

The example services listen on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```

# iRobot Create C++ Robot Raconteur Client Examples

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

### Windows

These instructions assume that a version of Visual Studio C++ is installed on the system. The
"Desktop Development with C++" workload must be installed in Visual Studio C++ to build the example.
CMake, Ninja, Git, and Python are also required. The easiest way to install all dependencies is to use Chocolatey.
See https://docs.chocolatey.org/en-us/choco/setup for instructions on installing Chocolatey.

Once Chocolatey, run the following in an elevated command prompt to install the required dependencies:

```cmd
choco install cmake ninja git visualstudio2022community visualstudio2022-workload-nativedesktop
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
vcpkg --triplet=x64-windows install robotraconteur
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
sudo apt install build-essential cmake git ninja-build robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev
```

Clone Robot Raconteur repository:

```cmd
git clone https://github.com/robotraconteur/robotraconteur.git
```

## Compiling

### Windows

```
cd robotraconteur\examples\simple_webcam\cpp\client
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cd robotraconteur/examples/simple_webcam/cpp/client
cmake -GNinja -S. -B build
cmake --build build
```

## Run Example

### Windows

Open a command prompt and start the appropriate iRobot Create service. For the C++ service run:

```cmd
cd robotraconteur\examples\irobot_create\cpp\service\build
irobot_create_service.exe COM4
```

In another command prompt and run the client:

```cmd
cd robotraconteur\examples\irobot_create\cpp\client\build
irobot_create_client.exe
```

### Ubuntu

Open a command prompt and start the appropriate iRobot Create service. For the C++ service run:

```bash
cd robotraconteur/examples/irobot_create/cpp/service/build
./irobot_create_service /dev/ttyUSB0
```

In another terminal and run the client:

```bash
cd robotraconteur/examples/irobot_create/cpp/client/build
./irobot_create_client
```

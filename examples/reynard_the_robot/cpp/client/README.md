# Reynard C++ Robot Raconteur Client Example

This example demonstrates controlling Reynard using C++ as a Robot Raconteur client. The example expects the
Python Reynard the Robot server to be running. The following instructions explain how to set up requirements,
build, and run the example. Instructions are provided for Windows and Ubuntu. These examples will work
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
sudo apt install build-essential cmake git ninja-build python3 python3-pip robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev
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
cd robotraconteur\examples\reynard_the_robot\cpp\client
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cd robotraconteur/examples/reynard_the_robot/cpp/client
cmake -GNinja -S. -B build
cmake --build build
```

## Run Example

### Windows

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot
```

In another command prompt and run the client:

```cmd
cd robotraconteur\examples\reynard_the_robot\cpp\client\build
reynard_robotraconteur_client.exe
```

### Ubuntu

Open a terminal and start the Reynard the Robot server:

```bash
python3 -m reynard_the_robot
```

In another terminal and run the client:

```bash
cd robotraconteur/examples/reynard_the_robot/cpp/client/build
./reynard_robotraconteur_client
```

# Simple Webcam C++ Robot Raconteur Client Examples

Several examples are provided for clients communicating with the simple webcam service. These examples expect
either the `simple_webcam_service` or `simple_webcam_service_multi` services to be running depending on the example.

- `simple_webcam_client` - (`simple_webcam_service_multi`) Connects to the multi camera service and captures a single frame
   from one or two cameras using the `capture_frame()` function member.
   The captured images are displayed for the user. Select one of the windows and press enter
   to close the window and exit the program.
- `simple_webcam_client_streaming` - (`simple_webcam_service`) Connects to the single camera service and streams images
   from the camera using the `frame_stream` pipe member.
   The images are displayed for the user. Press escape to close the window and exit the program.
- `simple_webcam_client_memory` - (`simple_webcam_service`) Connects to the single camera service and captures
   a frame using the `capture_frame_to_buffer()` function member. This function captures a frame and stores it to
   the buffers used by the `buffer` and `multidimbuffer` memory members. Segments of the memory members are displayed.
   This example is useful for demonstrating how to use memory members in Robot Raconteur.

These examples assume that the corresponding service is running. Any of the simple webcam service
examples should work regardless of language. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

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
vcpkg --triplet=x64-windows install robotraconteur opencv
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
sudo apt install build-essential cmake git ninja-build robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev libopencv-dev
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

Open a command prompt and start the appropriate simple webcam service. For the single camera C++ service run:

```cmd
cd robotraconteur\examples\simple_webcam\cpp\service\build
simple_webcam_service.exe
```

In another command prompt and run the client:

```cmd
cd robotraconteur\examples\simple_webcam\cpp\client\build
simple_webcam_client_streaming.exe
```

Replace `simple_webcam_client_streaming.exe` with the appropriate client executable for the example.

### Ubuntu

Open a terminal and start start the appropriate simple webcam service. For the single camera C++ service run:

```bash
cd robotraconteur/examples/simple_webcam/cpp/service/build
./simple_webcam_service
```

In another terminal and run the client:

```bash
cd robotraconteur/examples/simple_webcam/cpp/client/build
./simple_webcam_client_streaming
```

Replace `simple_webcam_client_streaming` with the appropriate client executable for the example.

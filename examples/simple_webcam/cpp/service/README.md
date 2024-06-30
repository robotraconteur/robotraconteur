# Simple Webcam C++ Robot Raconteur Service Examples

This directory contains examples of Robot Raconteur services with webcams. The services use OpenCV to capture
images from the webcam, and uses Robot Raconteur to provide the images to clients. Two services are provided,
a single camera service and a multi-camera service.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Practical systems should use the standard camera driver
[robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver)

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The single and multi camera services by default listen on TCP port 22355. Note that only
one of the services can be used at a time because they listen on the same TCP port. Use the following connection
information to connect to the services:

- Single Camera Service
  - URL: `rr+tcp://localhost:22355?service=webcam`
  - Node Name: `experimental.simplewebcam3`
  - Service Name: `webcam`
  - Root Object Type: `experimental.simplewebcam3.Webcam`
- Multi Camera Service
  - URL: `rr+tcp://localhost:22355?service=multiwebcam`
  - Node Name: `experimental.simplewebcam3_multi`
  - Service Name: `multiwebcam`
  - Root Object Type: `experimental.simplewebcam3.WebcamHost`

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
vcpkg --triplet=x64-windows install robotraconteur opencv drekar-launch-process-cpp
cd ..
```

### Ubuntu

The Robot Raconteur PPA must be configured to install Robot Raconteur. On Debian, the PPA is not available
and the [Robot Raconteur APT repository](https://github.com/robotraconteur/robotraconteur-apt) must be used.

```bash
sudo add-apt-repository ppa:robotraconteur/ppa
```

Install Robot Raconteur and the other dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake git ninja-build robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev libopencv-dev libdrekar-launch-process-cpp-dev
```

Clone Robot Raconteur repository:

```cmd
git clone https://github.com/robotraconteur/robotraconteur.git
```

## Compiling

### Windows

```
cd robotraconteur\examples\simple_webcam\cpp\service
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cd robotraconteur/examples/simple_webcam/cpp/service
cmake -GNinja -S. -B build
cmake --build build
```

## Run Example

### Windows

Open a command prompt and run the service:

```cmd
cd robotraconteur\examples\simple_webcam\cpp\service\build
simple_webcam_service.exe
```

Or

```cmd
cd robotraconteur\examples\simple_webcam\cpp\service\build
simple_webcam_service_multi.exe
```

The example services listen on port 22355. Run one of the client examples:

```cmd
cd robotraconteur\examples\simple_webcam\cpp\client\build
simple_webcam_client_streaming.exe
```

### Ubuntu

Open a terminal and run the service:

```bash
cd robotraconteur/examples/simple_webcam/cpp/service/build
./simple_webcam_service
```

Or

```bash
cd robotraconteur/examples/simple_webcam/cpp/service/build
./simple_webcam_service_multi
```

The example services listen on port 22355. Run one of the client examples:

```bash
cd robotraconteur/examples/simple_webcam/cpp/client/build
./simple_webcam_client_streaming
```

## Raspberry Pi arm64 Setup

### Start at boot

Ubuntu 22.04 arm64 is used to compile the C++ examples. Compiling for arm64 can be difficult so
binaries are provided on the wiki at
[Examples Binaries](https://github.com/robotraconteur/robotraconteur/wiki/Examples-Binaries).

First, copy the `simple_webcam_service` file to `$HOME`. Use `simple_webcam_service_multi` for multiple cameras.

Create the systemd unit file in `~/.config/systemd/user/simple-webcam-service.service`:

```ini
[Unit]
Description=Simple Webcam Robot Raconteur Service
After=network.target

[Service]
ExecStart=%h/simple_webcam_service

[Install]
WantedBy=default.target
```

It may be necessary to create the `~/.config/systemd/user` directory.

Enable and start the service:

```bash
systemctl --user enable simple-webcam-service
systemctl --user start simple-webcam-service
```

Allow the user to start services at boot:

```bash
sudo loginctl enable-linger $USER
```

Use the following command to see the status of the service:

```bash
systemctl --user status simple-webcam-service.service
```


It is recommended to make the SD card read only to prevent corruption. This can be done by modifying the
`/etc/fstab` and adding the `ro` (read only) option to the root partition.

An example `/etc/fstab` before modification:

```bash
LABEL=writable  /       ext4    discard,errors=remount-ro       0 1
LABEL=system-boot       /boot/firmware  vfat    defaults        0       1
```

and after modification:

```bash
LABEL=writable  /       ext4    discard,errors=remount-ro,ro    0 1
LABEL=system-boot       /boot/firmware  vfat    defaults        0       1
```

See
[Ubuntu Fstab](https://help.ubuntu.com/community/Fstab) for more information.

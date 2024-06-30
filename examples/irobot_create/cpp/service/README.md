# iRobot Create C++ Robot Raconteur Service Example

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
vcpkg --triplet=x64-windows install robotraconteur cereal drekar-launch-process-cpp
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
sudo apt install build-essential cmake git ninja-build robotraconteur-dev libssl-dev libdbus-1-dev libusb-1.0-0-dev libbluetooth-dev libcereal-dev libdrekar-launch-process-cpp-dev
```

Clone Robot Raconteur repository:

```cmd
git clone https://github.com/robotraconteur/robotraconteur.git
```
## Compiling

### Windows

```
cd robotraconteur\examples\irobot_create\cpp\service
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=..\..\..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

### Ubuntu
```
cd robotraconteur/examples/irobot_create/cpp/service
cmake -GNinja -S. -B build
cmake --build build
```

On Ubuntu it may be necessary to add the current user to the `dialout` group to access the serial port:

```bash
sudo usermod -a -G dialout $USER
```

It may be necessary to log out and log back in for the group change to take effect.

## Run Example

### Windows

Open a command prompt and run the service. The first argument is the serial port the Create is connected to. Use
the Device Manager in Windows to find the correct serial port.

```cmd
cd robotraconteur\examples\irobot_create\cpp\service\build
irobot_create_service.exe COM4
```

The example services listen on port 22354. Run one of the client examples:

```cmd
cd robotraconteur\examples\irobot_create\cpp\client\build
irobot_create_client.exe
```

### Ubuntu

Open a terminal and run the service. The first argument is the serial port the Create is connected to.

```bash
cd robotraconteur/examples/irobot_create/cpp/service/build
./irobot_create_service /dev/ttyUSB0
```

The example services listen on port 22354. Run one of the client examples:

```bash
cd robotraconteur/examples/irobot_create/cpp/client/build
./irobot_create_client
```

## Raspberry Pi arm64 Setup

### Start at boot

Ubuntu 22.04 arm64 is used to compile the C++ examples. Compiling for arm64 can be difficult so
binaries are provided on the wiki at
[Examples Binaries](https://github.com/robotraconteur/robotraconteur/wiki/Examples-Binaries).

First, copy the `irobot_create_service` file to `$HOME`.

Add your user to the `dialout` group to access the serial port:

```bash
sudo usermod -a -G dialout $USER
```

It may be necessary to log out and log back in for the group change to take effect.

Create the systemd unit file in `~/.config/systemd/user/irobot-create-service.service`:

```ini
[Unit]
Description=iRobot Create Robot Raconteur Service
After=network.target

[Service]
ExecStart=%h/irobot_create_service

[Install]
WantedBy=default.target
```

It may be necessary to create the `~/.config/systemd/user` directory.

This assumes that the default `/dev/ttyUSB0` serial port is used. If a different port is used, modify the
`ExecStart` line accordingly with the correct service device.

Enable and start the service:

```bash
systemctl --user enable irobot-create-service
systemctl --user start irobot-create-service
```

Allow the user to start services at boot:

```bash
sudo loginctl enable-linger $USER
```

Use the following command to see the status of the service:

```bash
systemctl --user status irobot-create-service.service
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

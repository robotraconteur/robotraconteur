<p align="center"><img src="https://robotraconteurpublicfiles.s3.amazonaws.com/RRheader2.jpg"></p>

# Robot Raconteur Core Library and Wrappers

![CI Build Status](https://github.com/robotraconteur/robotraconteur/workflows/CI/badge.svg)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason, "Robot Raconteur® version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things", in Proc. IEEE Conference on Automation Science and Engineering, 2016, pp. 595-602.](https://s3.amazonaws.com/robotraconteurpublicfiles/docs/RobotRaconteur_CASE2016.pdf)

## Documentation

See [https://github.com/robotraconteur/robotraconteur/wiki/Documentation](https://github.com/robotraconteur/robotraconteur/wiki/Documentation) for documentation.

## Getting Help

1. I found a bug! Please leave an issue on the [GitHub Issues](https://github.com/robotraconteur/robotraconteur/issues) page
2. I have a specific question about how to use Robot Raconteur: Please leave a question on [GitHub Discussions Q&A](https://github.com/robotraconteur/robotraconteur/discussions/categories/q-a)
3. I have a general question or comment: Please leave a message on [GitHub Discussions](https://github.com/robotraconteur/robotraconteur/discussions).

## Quick Start

A simple service will initialize Robot Raconteur and register an object as a service. This example service creates a simple service that contains a single function to drive an iRobot Create through a serial port. This is a minimal subset of the full example in the documentation.

`minimalcreateservice.py`

    import RobotRaconteur as RR
    RRN=RR.RobotRaconteurNode.s
    import threading
    import serial
    import struct

    minimal_create_interface="""
    service experimental.minimal_create

    object create_obj
        function void Drive(int16 velocity, int16 radius)
    end object
    """

    class create_impl(object):
        def __init__(self, port):
            self._lock=threading.Lock()
            self._serial=serial.Serial(port=port,baudrate=57600)
            dat=struct.pack(">4B",128,132,150, 0)
            self._serial.write(dat)

        def Drive(self, velocity, radius):
            with self._lock:
                dat=struct.pack(">B2h",137,velocity,radius)
                self._serial.write(dat)

    with RR.ServerNodeSetup("experimental.minimal_create", 52222):
        #Register the service type
        RRN.RegisterServiceType(minimal_create_interface)

        create_inst=create_impl("/dev/ttyUSB0")

        #Register the service
        RRN.RegisterService("Create","experimental.minimal_create.create_obj",create_inst)

        #Wait for program exit to quit
        input("Press enter to quit")

This service can now be called by a connecting client. Because of the magic of Robot Raconteur, it is only necessary to connect to the service to utilize its members. In Python and MATLAB there is no boilerplate code, and in the other languages the boilerplate code is generated automatically.

`minimalcreateclient.py`

    from RobotRaconteur.Client import *
    import time

    #RRN is imported from RobotRaconteur.Client
    #Connect to the service.
    obj=RRN.ConnectService('rr+tcp://localhost:52222/?service=Create')

    #The "Create" object reference is now available for use
    #Drive for a bit
    obj.Drive(100,5000)
    time.sleep(1)
    obj.Drive(0,5000)

In MATLAB, this client is even simpler.

`minimalcreateclient.m`

    o=RobotRaconteur.Connect('rr+tcp://localhost:52222/?service=Create');
    o.Drive(int16(100),int16(5000));
    pause(1);
    o.Drive(int16(0),int16(0));

## Installation

### Windows

#### C++

`vcpkg` is used to install the Robot Raconteur C++ library. See https://github.com/microsoft/vcpkg for installation instructions.

To build Robot Raconteur, clone the `vcpkg-robotraconteur` overlay repo in the vcpkg directory:

```
git clone https://github.com/robotraconteur/vcpkg-robotraconteur.git
```

and build the library:

```
vcpkg --overlay-ports=vcpkg-robotraconteur\ports install robotraconteur
```

To build x64, use:

```
vcpkg --overlay-ports=vcpkg-robotraconteur\ports install robotraconteur:x64-windows
```

#### Python

The Python wrappers are distributed using PyPi.

```
pip install robotraconteur
```

#### C\# 

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

#### Java

The Java library is available on the releases page on github.

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

### Ubuntu Xenial, Bionic, and Focal

A PPA is available for Robot Raconteur. [https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa](https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa)

```
sudo add-apt-repository ppa:robotraconteur/ppa
sudo apt-get update
```

#### C++

```
sudo apt-get install robotraconteur-dev
```

#### Python
```
sudo apt-get install python-robotraconteur
sudo apt-get install python3-robotraconteur

```

#### C\# 

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

The Linux native library must be installed using apt:

```
sudo apt-get install librobotraconteur-net-native
```

#### Java

Java must be built from source

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

### Debian 10 (buster)

An apt repository is available for Debian. Packages are available for amd64, armhf, and arm64. See below for raspbian setup. To use, run:

```
sudo apt install apt-transport-https dirmngr gnupg ca-certificates
wget -O - https://robotraconteur.github.io/robotraconteur-apt/wasontech-apt.gpg.key | sudo apt-key add -
echo "deb https://robotraconteur.github.io/robotraconteur-apt/debian buster main" | sudo tee /etc/apt/sources.list.d/robotraconteur.list
sudo apt update
```

#### C++

```
sudo apt-get install robotraconteur-dev
```

#### Python
```
sudo apt-get install python-robotraconteur
sudo apt-get install python3-robotraconteur

```

#### C\# 

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

The Linux native library must be installed using apt:

```
sudo apt-get install librobotraconteur-net-native
```

#### Java

Java must be built from source

### Raspbian 10 (buster)

The Raspberry Pi OS (raspbian) for amhf 32-bit processors is slightly different than the main debian armhf distributions. It uses ARMv6 instructions, instead of the ARMv7 instructions used by the main debian installation. Use the following to set up the raspbian repository, and see the debian section for the rest of the instructions.

**Raspberry Pi OS (raspbian) for 64-bit uses the standard debian repository. This is only for 32-bit ARMv6 installation.**

```
sudo apt install apt-transport-https dirmngr gnupg ca-certificates
wget -O - https://robotraconteur.github.io/robotraconteur-apt/wasontech-apt.gpg.key | sudo apt-key add -
echo "deb https://robotraconteur.github.io/robotraconteur-apt/raspbian buster main" | sudo tee /etc/apt/sources.list.d/robotraconteur.list
sudo apt update
```

### Mac OSX

#### C++

Use `brew` to install the Robot Raconteur C++ library.

```
brew install robotraconteur/robotraconteur/robotraconteur
```

#### Python

The Python wrappers are distributed using PyPi.

```
pip install robotraconteur
```

#### C\# 

The C\# library must be built from source.

#### Java

The Java library is available to download from the github release.

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

## Building

The software is tested using GitHub Actions continuous integration. See .github/workflows/main.yml for more information.

### Windows

Building the core library requires Visual Studio 2012 through 2019, Boost 1.72.0, and CMake. Follow the instructions on the Boost website to build Boost. Alternatively, Boost can be built using the [vcpkg](https://github.com/Microsoft/vcpkg) utility.

### Ubuntu Focal
Install the dependencies:

```
apt-get install default-jdk default-jdk-headless default-jre default-jre-headless zlib1g zlib1g-dev libssl-dev libusb-1.0-0 libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev git cmake g++ make libboost-all-dev autoconf automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel mono-devel
```

To build:

```
mkdir build && cd build
cmake -DBUILD_GEN=ON -DBUILD_TEST=ON -DBoost_USE_STATIC_LIBS=OFF ..
make
```

### Ubuntu Bionic
Install the dependencies:

```
apt-get install default-jdk default-jdk-headless default-jre default-jre-headless python2.7-dev libpython2.7-dev zlib1g zlib1g-dev libssl-dev libusb-1.0-0 libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev python-numpy python-setuptools python-wheel git cmake g++ make libboost-all-dev autoconf automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel mono-devel
```

To build:

```
mkdir build && cd build
cmake -DBUILD_GEN=ON -DBUILD_TEST=ON -DBoost_USE_STATIC_LIBS=OFF ..
make
```
    
### Mac OSX

Robot Raconteur requires XCode, CMake, Boost Libraries, and OpenSSL.
 
Use homebrew to install boost and openssl:

```
brew install boost
brew install openssl
```

Note that homebrew does not create symlinks, so the CMake OPENSSL_ROOT_DIR must be set to the cellar folder. 

Configure build for XCode using CMake GUI. Open resulting project file and build.

### Swig
Python, Java, and C# use SWIG to generate language bindings. Robot Raconteur currently requires SWIG version 4.0.2 or higher. Windows
can download from http://swig.org, or install from chocolatey using

    choco install swig

Mac OSX can install using

    brew install swig

Ubuntu must build SWIG from source. See 
https://github.com/swig/swig/wiki/Getting-Started for instructions.

### MATLAB Mex

MATLAB install required for build. CMake FindMatlab module is used to locate the MATLAB build dependencies. Be sure to build the MEX file against static boost libraries. Linux will require Boost to be built from source with "-fPIC" C++ gcc option. The apt repository static libraries cannot be used because they were not built with "-fPIC".  

## ROS Support

Robot Raconteur can be built and utilized from within ROS. To use, clone the robotraconteur repository into catkin_ws/src, run rosdep to install dependencies, and use catkin_make_isolated or catkin_tools to build. By default, the resulting build will not have ROS support and will only build the core static library. To build with ROS support, RobotRaconteurGen, and Python, the CMake option `ROBOTRACONTEUR_ROS` must be set.

Before building, run `rosdep`:

    rosdep install --from-paths src --ignore-src -r -y

To build with `catkin_make_isolated`:

    catkin_make_isolated --cmake-args -DROBOTRACONTEUR_ROS=1

To build with `catkin_tools`:

    catkin config --cmake-args -DROBOTRACONTEUR_ROS=1
    catkin build

Note that SWIG version 4.0.2 MUST be installed before attempting to build the Python bindings. Ubuntu includes older versions of SWIG in the apt repository. Robot Raconteur will not build using theese older versions installed from apt! Install to /usr/local so catkin can find the swig executable. See [here](https://github.com/swig/swig/wiki/Getting-Started) for SWIG installation instructions.

## Standard Service Types

The Robot Raconteur project has defined a number of standard service definitions that contain numerous structure, pod, namedarray, and object types. These types cover a range of common data types, and provide standardized interfaces to devices. These types should be used whenever possible so that services will be interoperable. The standard service types are available in the https://github.com/robotraconteur/robotraconteur_standard_robdef GitHub repository.

## Companion Libraries

The Robot Raconteur Companion libraries are provided to assist in using the standard service types, along with other generic utility functions. Currently, the companion libraries contain the standard service types, info file loaders, and general utility functions. The following libraries are available:

* Python: https://github.com/robotraconteur/robotraconteur_companion_python
* C++: https://github.com/robotraconteur/robotraconteur_companion
* C#: https://github.com/robotraconteur/RobotRaconteurNET.Companion

The Python companion library can also be installed using `pip install RobotRaconteurCompanion`

See https://github.com/robotraconteur-contrib/robotraconteur_camera_driver/blob/master/robotraconteur_camera_driver.py for an example utilizing standard types and the companion library.

## Robot Raconteur Directory

The Robot Raconteur project maintains a list of available drivers. The directory can be found here:

https://github.com/robotraconteur/robotraconteur-directory

## License

The Robot Raconteur core library is Apache 2.0 licensed.

"Robot Raconteur" and the Robot Raconteur logo are registered trademarks of Wason Technology, LLC. All rights reserved.

Robot Raconteur is covered United States Patent No. 10536560

Robot Raconteur is developed by John Wason, PhD, Wason Technology, LLC

## Acknowledgment

This work was supported in part by Subaward No. ARM-TEC-18-01-F-19 and ARM-TEC-19-01-F-24 from the Advanced Robotics for Manufacturing ("ARM") Institute under Agreement Number W911NF-17-3-0004 sponsored by the Office of the Secretary of Defense. ARM Project Management was provided by Christopher Adams. The views and conclusions contained in this document are those of the authors and should not be interpreted as representing the official policies, either expressed or implied, of either ARM or the Office of the Secretary of Defense of the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Government purposes, notwithstanding any copyright notation herein.

This work was supported in part by the New York State Empire State Development Division of Science, Technology and Innovation (NYSTAR) under contract C160142. 

![](docs/figures/arm_logo.jpg) ![](docs/figures/nys_logo.jpg)


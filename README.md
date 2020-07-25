<p align="center"><img src="https://robotraconteurpublicfiles.s3.amazonaws.com/RRheader2.jpg"></p>

# Robot Raconteur Core Library and Wrappers

![CI Build Status](https://github.com/robotraconteur/robotraconteur/workflows/CI/badge.svg)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason, "Robot Raconteur® version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things", in Proc. IEEE Conference on Automation Science and Engineering, 2016, pp. 595-602.](https://s3.amazonaws.com/robotraconteurpublicfiles/docs/RobotRaconteur_CASE2016.pdf)

See [http://robotraconteur.com/documentation](http://robotraconteur.com/documentation) for documentation.

## Getting Help

1. I found a bug! Please leave an issue on the [GitHub Issues](https://github.com/robotraconteur/robotraconteur/issues) page
2. I have a specific question about how to use Robot Raconteur: Please leave a question on [Stack Overflow](https://stackoverflow.com/) with "Robot Raconteur" in the title.
3. I have a general question or comment: Please leave a message on the [Robot Raconteur Forum](https://www.robotraconteur.com/forum).

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

The MATLAB library is available on the releases page on github. Unzip the contents of the archive and add the directory to the MATLAB path.

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

C\# must be built from source

#### Java

Java must be built from source

#### MATLAB

The MATLAB library is available on the releases page on github. Unzip the contents of the archive and add the directory to the MATLAB path.

### Mac OSX

#### C++

Use `brew` to install the Robot Raconteur C++ library.

```
brew install johnwason/robotraconteur/robotraconteur
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

The MATLAB library is available on the releases page on github. Unzip the contents of the archive and add the directory to the MATLAB path.

## Building

The software is tested using Travis CI continuous integration. See travis.yml for more information.

### Windows

Building the core library requires Visual Studio 2012 through 2019, Boost 1.72.0, and CMake. Follow the instructions on the Boost website to build Boost. Alternatively, Boost can be built using the [vcpkg](https://github.com/Microsoft/vcpkg) utility.

### Ubuntu Bionic
Install the dependencies:

```
apt-get install default-jdk default-jdk-headless default-jre default-jre-headless python2.7-dev libpython2.7-dev zlib1g zlib1g-dev libssl-dev libusb-1.0-0 libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev python-numpy python-setuptools python-wheel git cmake g++ make libboost-all-dev autoconf automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel mono-devel -qq
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
Python, Java, and C# use SWIG to generate language bindings. Robot Raconteur currently uses SWIG version 4.0.0 or higher. Windows
can download from http://swig.org, or install from chocolatey using

    choco install swig

Mac OSX can install using

    brew install swig

Ubuntu Focal or higher can install using

   apt install swig

Ubuntu Bionic and Kinetic must build SWIG from source. See 
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

Note that SWIG version 4.0.0 MUST be installed before attempting to build the Python bindings. Ubuntu Focal includes SWIG 4.0.0, but older versions of Ubuntu do not. Robot Raconteur will not build using theese older versions installed from apt! Install to /usr/local so catkin can find the swig executable. See [here](https://github.com/swig/swig/wiki/Getting-Started) for SWIG installation instructions.
    
## Documentation

Documentation for version 0.8.1 can be found at [https://robotraconteur.com/documentation](https://robotraconteur.com/documentation). Documentation is being updated.

## License

The Robot Raconteur core library is Apache 2.0 licensed.

"Robot Raconteur" and the Robot Raconteur logo are registered trademarks of Wason Technology, LLC. All rights reserved.

Robot Raconteur is covered United States Patent No. 10536560

Robot Raconteur is developed by John Wason, PhD, Wason Technology, LLC

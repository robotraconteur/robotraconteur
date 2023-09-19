<p align="center"><img src="https://robotraconteurpublicfiles.s3.amazonaws.com/RRheader2.jpg"></p>

# Robot Raconteur Core Library and Wrappers

![CI Build Status](https://github.com/robotraconteur/robotraconteur/workflows/CI/badge.svg)
![license - apache 2.0](https://img.shields.io/:license-Apache%202.0-yellowgreen.svg)
![Python](https://img.shields.io/badge/python-2.7+|3.5+-blue.svg)
![pypi](https://img.shields.io/pypi/v/robotraconteur)
![C++](https://img.shields.io/badge/C++-98+-blue.svg?style=flat&logo=c%2B%2B)
![C\#](https://img.shields.io/badge/C%23-4.5|netstandard2.0-blue.svg?style=flat&logo=c-sharp)
![nuget](https://img.shields.io/nuget/v/robotraconteurnet)
![Java](https://img.shields.io/badge/java-%23ED8B00.svg?style=flat&logo=java)
[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)
![conda](https://img.shields.io/conda/vn/robotraconteur/robotraconteur?label=conda)

![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)
![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Android](https://img.shields.io/badge/Android-3DDC84?style=for-the-badge&logo=android&logoColor=white)
![IOS](https://img.shields.io/badge/iOS-000000?style=for-the-badge&logo=ios&logoColor=white)
![Mac OS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)
![Raspberry Pi](https://img.shields.io/badge/-RaspberryPi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![FreeBSD](https://img.shields.io/badge/-FreeBSD-%23870000?style=for-the-badge&logo=freebsd&logoColor=white)
![ROS](https://img.shields.io/badge/-ROS-22314E?style=for-the-badge&logo=ros&logoColor=white)
![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason, "Robot Raconteur® version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things", in Proc. IEEE Conference on Automation Science and Engineering, 2016, pp. 595-602.](https://s3.amazonaws.com/robotraconteurpublicfiles/docs/RobotRaconteur_CASE2016.pdf)

## Contents

- [Robot Raconteur Core Library and Wrappers](#robot-raconteur-core-library-and-wrappers)
  - [Documentation](#documentation)
  - [Getting Help](#getting-help)
  - [Quick Start](#quick-start)
  - [Getting Started](#getting-started)
  - [Installation](#installation)
  - [Building](#building)
  - [ROS Support](#ros-support)
  - [Standard Service Types](#standard-service-types)
  - [Companion Libraries](#companion-libraries)
  - [Robot Raconteur Directory](#robot-raconteur-directory)
  - [Package Quality](#package-quality)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgment](#acknowledgment)


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

## Getting Started

Robot Raconteur has a large ecosystem with a number of related projects. Start with the Robot Raconteur tutorial,
and the Python examples:

https://robotraconteur.github.io/robotraconteur/doc/core/latest/python/

https://github.com/robotraconteur/RobotRaconteur_Python_Examples

The Python examples support using a Gazebo simulated iRobot Create robot. See the training simulator for installation
instructions:

https://github.com/robotraconteur-contrib/robotraconteur_training_sim

The training simulator also contains a simulation for two Universal Robots UR5e robots, with grippers and cameras. 
Example scripts to control the robots are included. See the training simulator readme for instructions.

Next, take a look out the examples for other languages:

https://github.com/robotraconteur/RobotRaconteur_CPP_Examples

https://github.com/robotraconteur/RobotRaconteur_CSharp_Examples

https://github.com/robotraconteur/RobotRaconteur_Java_Examples

https://github.com/robotraconteur/RobotRaconteur_MATLAB_Examples

Robot Raconteur provides a large number of standardized types to use with robots and other devices. See the 
standard robdef repository:

https://github.com/robotraconteur/robotraconteur_standard_robdef

There are numerous other projects, drivers, and resources in the ecosystem. See the directory
for a full list of available resources:

https://github.com/robotraconteur/robotraconteur-directory

Also checkout the PyRI Open source teach pendant: https://github.com/pyri-project/pyri-core

## Installation

See [docs/common/installation.md](docs/common/installation.md) for installation instructions. 

The following platforms are supported:

* **Windows** (x86, amd64, arm64): C++, Python, C\#, Java, MATLAB, LabView
* **Linux** (x86, x64, armhf, arm64): C++, Python, C\#, Java, MATLAB, LabView
* **MacOS** (x64, arm64): C++, Python, C\#, Java, MATLAB
* **Android** (x86, x64, armhf, arm64): C++, Java
* **iOS** (arm-v7, arm64): C++
* **Browser** (Chrome, Firefox, Edge, Safari): C++, Python, JavaScript
* **FreeBSD** (x64)

## Building

See [docs/common/building.md](docs/common/building.md) for build instructions. 

## ROS Support

Robot Raconteur is available in ROS Noetic and ROS Humble using the `robotraconteur` package. These packages are built
using the `ros-noetic` and `ros2-humble` branches. The `ros2-humble` branch should work with other versions of 
ROS 2, but swig version 4.0.2 or greater must be installed first.

A Robot Raconteur to ROS 2 bridge is available, allowing access to ROS 2 topics and services from Robot Raconteur:

https://github.com/robotraconteur-contrib/robotraconteur_ros2_bridge

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

## Package Quality

Robot Raconteur Core is a ROS Quality Level 2 package. See the [Quality Declaration](QUALITY_DECLARATION.md) for more details.

## Contributing

Contributors must sign a Contributor License Agreement (CLA). Please see https://www.wasontech.com/contributors to 
complete and return a signed agreement. Wason Technology, LLC uses the Harmony CLA (https://www.harmonyagreements.org/).

## License

The Robot Raconteur core library is Apache 2.0 licensed.

"Robot Raconteur" and the Robot Raconteur logo are registered trademarks of Wason Technology, LLC. All rights reserved.

Robot Raconteur is covered United States Patent No. 10536560

Robot Raconteur is developed by John Wason, PhD, Wason Technology, LLC

## Acknowledgment

This work was supported in part by Subaward No. ARM-TEC-18-01-F-19 and ARM-TEC-19-01-F-24 from the Advanced Robotics for Manufacturing ("ARM") Institute under Agreement Number W911NF-17-3-0004 sponsored by the Office of the Secretary of Defense. ARM Project Management was provided by Christopher Adams. The views and conclusions contained in this document are those of the authors and should not be interpreted as representing the official policies, either expressed or implied, of either ARM or the Office of the Secretary of Defense of the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Government purposes, notwithstanding any copyright notation herein.

This work was supported in part by the New York State Empire State Development Division of Science, Technology and Innovation (NYSTAR) under contract C160142. 

![](docs/figures/arm_logo.jpg) ![](docs/figures/nys_logo.jpg)


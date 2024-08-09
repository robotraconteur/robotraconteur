<p align="center"><img src="docs/figures/logo-header.svg"></p>

# Robot Raconteur Core Library and Wrappers

[![CI](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml/badge.svg)](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml)
![license - apache 2.0](https://img.shields.io/:license-Apache%202.0-yellowgreen.svg)
![Python](https://img.shields.io/badge/python-2.7+|3.5+-blue.svg?style=flat&logo=python&logoColor=ffdd54)
![pypi](https://img.shields.io/pypi/v/robotraconteur?style=flat&logo=pypi)
![C++](https://img.shields.io/badge/C++-98+-blue.svg?style=flat&logo=c%2B%2B)
![C\#](https://img.shields.io/badge/C%23-4.5|netstandard2.0-blue.svg?style=flat&logo=csharp)
![nuget](https://img.shields.io/nuget/v/robotraconteurnet?style=flat&logo=nuget)
![Java](https://img.shields.io/badge/Java-8+-blue.svg?style=flat&logo=openjdk)
[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)
![conda](https://img.shields.io/conda/vn/conda-forge/robotraconteur?label=conda&logo=anaconda)
![ros-noetic](https://img.shields.io/ros/v/noetic/robotraconteur?styple=flat&logo=ros)
![ros-humble](https://img.shields.io/ros/v/humble/robotraconteur?styple=flat&logo=ros)
![ros-iron](https://img.shields.io/ros/v/iron/robotraconteur?styple=flat&logo=ros)
![ros-jazzy](https://img.shields.io/ros/v/jazzy/robotraconteur?styple=flat&logo=ros)

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

[J. Wason and J. T. Wen, "Robot Raconteur® Updates on an Open Source Interoperable Middleware for Robotics", in Proc. IEEE Conference on Automation Science and Engineering, 2023, pp. 1-8.](https://files2.wasontech.com/RobotRaconteur_CASE2023.pdf)

[H. He, B. Aksoy, G. Saunders, J. Wason, and J. T. Wen, "Plug-and-play software architecture for coordinating multiple industrial robots and sensors from multiple vendors", in Proc. IEEE Conference on Automation Science and Engineering, 2023, pp. 1-8.](https://files2.wasontech.com/RobotRaconteur_CASE2023_plugandplay.pdf)

[J. Wason, "Robot Raconteur® version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things", in Proc. IEEE Conference on Automation Science and Engineering, 2016, pp. 595-602.](https://files2.wasontech.com/RobotRaconteur_CASE2016.pdf)

[J. Wason and J. T. Wen, "Robot Raconteur: A Communication Architecture and Library for Robotic and Automation Systems", in Proc. IEEE Conference on Automation Science and Engineering, 2011, pp. 761-766.](https://files2.wasontech.com/RobotRaconteur_CASE2011.pdf)

See the [Getting Started Guide](https://robotraconteur.github.io/robotraconteur/doc/core/latest/getting_started/)!

## Contents

- [Robot Raconteur Core Library and Wrappers](#robot-raconteur-core-library-and-wrappers)
  - [Documentation](#documentation)
  - [Examples](#examples)
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

## Examples

See the [examples](examples) directory for examples in various programming languages.

## Getting Help

1. I found a bug! Please leave an issue on the [GitHub Issues](https://github.com/robotraconteur/robotraconteur/issues) page
2. I have a specific question about how to use Robot Raconteur: Please leave a question on [GitHub Discussions Q&A](https://github.com/robotraconteur/robotraconteur/discussions/categories/q-a)
3. I have a general question or comment: Please leave a message on [GitHub Discussions](https://github.com/robotraconteur/robotraconteur/discussions).

## Quick Start

The Quick Start example demonstrates the basic functionality of Robot Raconteur be creating a service,
and then calling the service using a client. This example uses the "Reynard the Robot" Python package,
which provides a simple cartoon robot.

Before running the example, make sure to install the required packages:

```bash
python -m pip install robotraconteur reynard-the-robot
```

On Linux, use `python3` instead of `python` to run the Python 3 interpreter. Use `python3` in the rest
of the examples as well.

`reynard_quickstart_service.py`

```python
import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s

import threading
import reynard_the_robot

# Define the service definition for the quickstart service
reynard_quickstart_interface = """
service experimental.reynard_quickstart

object ReynardQuickstart
    function void say(string text)
    function void teleport(double x, double y)
end
"""

# Implement the quickstart service


class ReynardQuickstartImpl(object):
    def __init__(self):
        self.reynard = reynard_the_robot.Reynard()
        self.reynard.start()
        self._lock = threading.Lock()

    def say(self, text):
        with self._lock:
            self.reynard.say(text)

    def teleport(self, x, y):
        with self._lock:
            self.reynard.teleport(x, y)


with RR.ServerNodeSetup("experimental.minimal_create2", 53222):
    # Register the service type
    RRN.RegisterServiceType(reynard_quickstart_interface)

    reynard_inst = ReynardQuickstartImpl()

    # Register the service
    RRN.RegisterService("reynard", "experimental.reynard_quickstart.ReynardQuickstart", reynard_inst)

    # Wait for program exit to quit
    input("Press enter to quit")
```

To run the service, execute the following command:

```bash
python reynard_quickstart_service.py
```

And open a browser to [http://localhost:29201](http://localhost:29201) to view the Reynard user interface.

This service can now be called by a connecting client. Because of the magic of Robot Raconteur, it is only necessary to connect to the service to utilize its members. In Python and MATLAB there is no boilerplate code, and in the other languages the boilerplate code is generated automatically.

`reynard_quickstart_client.py`

```python
from RobotRaconteur.Client import *

# RRN is imported from RobotRaconteur.Client
# Connect to the service.
obj = RRN.ConnectService('rr+tcp://localhost:53222/?service=reynard')

# Call the say function
obj.say("Hello from Reynard!")

# Call the teleport function
obj.teleport(100, 200)
```

To run the client, execute the following command:

```bash
python reynard_quickstart_client.py
```

The MATLAB Add-On for Robot Raconteur can be installed using the Add-On Explorer in MATLAB and searching for "Robot Raconteur".

In MATLAB, the example client is even simpler.

`reynard_quickstart_client.m`

```matlab
% Connect to the service
o = RobotRaconteur.ConnectService('rr+tcp://localhost:53222/?service=reynard');

% Call the say function
o.say("Hello from MATLAB!");

% Call the teleport function
o.teleport(-150,200);
```

The quickstart file can be found in the [examples/quickstart](examples/quickstart) directory.

## Getting Started

Robot Raconteur has a large ecosystem with a number of related projects. Start with the
[Robot Raconteur Getting Started Guide](https://robotraconteur.github.io/robotraconteur/doc/core/latest/getting_started/).

The Python examples support using a Gazebo simulated iRobot Create robot. See the training simulator for installation
instructions:

https://github.com/robotraconteur-contrib/robotraconteur_training_sim

The training simulator also contains a simulation for two Universal Robots UR5e robots, with grippers and cameras.
Example scripts to control the robots are included. See the training simulator readme for instructions.

Numerous examples can be found in the [examples/](https://github.com/robotraconteur/robotraconteur/tree/master/examples)

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

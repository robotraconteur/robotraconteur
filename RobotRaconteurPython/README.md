<p align="center"><img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/docs/figures/logo-header.svg"></p>

# robotraconteur Python Package

![Python](https://img.shields.io/badge/python-2.7+|3.5+-blue.svg?style=flat&logo=python&logoColor=ffdd54)
![pypi](https://img.shields.io/pypi/v/robotraconteur?style=flat&logo=pypi)
![conda](https://img.shields.io/conda/vn/conda-forge/robotraconteur?label=conda&logo=anaconda)
[![CI](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml/badge.svg)](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason and J. T. Wen, "Robot Raconteur Updates on an Open Source Interoperable Middleware for Robotics", in Proc. IEEE Conference on Automation Science and Engineering, 2023, pp. 1-8.](https://files2.wasontech.com/RobotRaconteur_CASE2023.pdf)

Github Repository: https://github.com/robotraconteur/robotraconteur

## Introduction

Robot Raconteur is a powerful communication framework for robotics and automation systems. While intended for use with robotics, it is flexible enough to be used for other applications, including building control, infrastructure, and Internet-of-Things applications, among many others. This package
contains the Robot Raconteur Core library for Python.

## Installation

For most platforms, the easiest way to install Robot Raconteur is using `pip`:

```bash
python -m pip install robotraconteur
```

Packages are available from pip for Python 3.6 and later for Windows (x86,x64), macOS (x64,arm64) and Linux (x64,arm64).

Use `python3` instead of `python` if you are using Ubuntu or other Linux distributions.

It may be neccessary to update `pip` before installing on older Linux distributions:

```bash
sudo python3 -m pip install --upgrade pip
```

Conda packages are also available from the `conda-forge` channel:

```bash
conda install -c conda-forge robotraconteur
```

It is highly recommended that the `robotraconteurcompanion` package is also installed.

```bash
python -m pip install robotraconteur robotraconteurcompanion
```

Or with Conda:

```bash
conda install -c conda-forge robotraconteur robotraconteur_companion_python
```

For other installation options, see the [installation instructions](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md).

## Documentation

See the [Robot Raconteur Documentation](https://github.com/robotraconteur/robotraconteur/wiki/Documentation)

See the [Robot Raconteur Examples](https://github.com/robotraconteur/robotraconteur/tree/master/examples)

## Quick Start

The Quick Start example demonstrates the basic functionality of Robot Raconteur be creating a service,
and then calling the service using a client. This example uses the "Reynard the Robot" Python package,
which provides a simple cartoon robot.

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

## License

Apache 2.0

## Support

Please report bugs and issues on the [GitHub issue tracker](https://github.com/robotraconteur/robotraconteur/issues).

Ask questions on the [Github discussions](https://github.com/robotraconteur/robotraconteur/discussions).

## Acknowledgment

This work was supported in part by Subaward No. ARM-TEC-18-01-F-19 and ARM-TEC-19-01-F-24 from the Advanced Robotics for Manufacturing ("ARM") Institute under Agreement Number W911NF-17-3-0004 sponsored by the Office of the Secretary of Defense. ARM Project Management was provided by Christopher Adams. The views and conclusions contained in this document are those of the authors and should not be interpreted as representing the official policies, either expressed or implied, of either ARM or the Office of the Secretary of Defense of the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Government purposes, notwithstanding any copyright notation herein.

This work was supported in part by the New York State Empire State Development Division of Science, Technology and Innovation (NYSTAR) under contract C160142.

![](https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/docs/figures/logo-header.svg)

# RobotRaconteurNET NuGet Package

![C\#](https://img.shields.io/badge/C%23-4.5|netstandard2.0-blue.svg?style=flat&logo=csharp)
![nuget](https://img.shields.io/nuget/v/robotraconteurnet?style=flat&logo=nuget)
[![CI](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml/badge.svg)](https://github.com/robotraconteur/robotraconteur/actions/workflows/main.yml)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason and J. T. Wen, "Robot Raconteur Updates on an Open Source Interoperable Middleware for Robotics", in Proc. IEEE Conference on Automation Science and Engineering, 2023, pp. 1-8.](https://files2.wasontech.com/RobotRaconteur_CASE2023.pdf)

Github Repository: https://github.com/robotraconteur/robotraconteur

## Introduction

Robot Raconteur is a powerful communication framework for robotics and automation systems. While intended for use with robotics, it is flexible enough to be used for other applications, including building control, infrastructure, and Internet-of-Things applications, among many others. This package
contains the Robot Raconteur Core library for .NET (C\#).

## Installation

For most platforms, the easiest way to install Robot Raconteur is using `dotnet`:

```bash
dotnet add package RobotRaconteurNET
```

## Documentation

See the [Robot Raconteur Documentation](https://github.com/robotraconteur/robotraconteur/wiki/Documentation)

See the [Robot Raconteur Examples](https://github.com/robotraconteur/robotraconteur/tree/master/examples)

## Example

This example demonstrates a simple client for the Reynard the Robot cartoon robot. See
[Reynard the Robot](https://github.com/robotraconteur/reynard-the-robot) for more information
and setup instructions.

In a terminal,run the following command to start the Reynard the Robot server:

```bash
python -m reynard_the_robot
```

On Linux, you may need to use `python3` instead of `python`.

Open a browser to [http://localhost:29201](http://localhost:29201) to view the Reynard user interface.

The following is an example C\# client for Reynard the Robot:

```csharp
using System;
using RobotRaconteur;
using System.Threading;

// Initialize the client node
using (var node_setup = new ClientNodeSetup())
{
    // Connect to the Reynard service using a URL
    var c = (experimental.reynard_the_robot.Reynard)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:29200?service=reynard");

    // Teleport the robot
    c.teleport(0.1, -0.2);

    // Drive the robot with no timeout
    c.drive_robot(0.5, -0.2, -1, false);

    // Wait for one second
    RobotRaconteurNode.s.Sleep(1000);

    // Stop the robot
    c.drive_robot(0, 0, -1, false);

    // Set the arm position
    c.setf_arm_position(100.0 * (Math.PI / 180), -30 * (Math.PI / 180), -70 * (Math.PI / 180));

    //  Set the color to red
    c.color = new double[] { 1.0, 0.0, 0.0 };

    // Say hello
    c.say("Hello, World From C#!");
}
```

## License

Apache 2.0

## Support

Please report bugs and issues on the [GitHub issue tracker](https://github.com/robotraconteur/robotraconteur/issues).

Ask questions on the [Github discussions](https://github.com/robotraconteur/robotraconteur/discussions).

## Acknowledgment

This work was supported in part by Subaward No. ARM-TEC-18-01-F-19 and ARM-TEC-19-01-F-24 from the Advanced Robotics for Manufacturing ("ARM") Institute under Agreement Number W911NF-17-3-0004 sponsored by the Office of the Secretary of Defense. ARM Project Management was provided by Christopher Adams. The views and conclusions contained in this document are those of the authors and should not be interpreted as representing the official policies, either expressed or implied, of either ARM or the Office of the Secretary of Defense of the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Government purposes, notwithstanding any copyright notation herein.

This work was supported in part by the New York State Empire State Development Division of Science, Technology and Innovation (NYSTAR) under contract C160142.

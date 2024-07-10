# Standard Robot Raconteur Devices Examples

Robot Raconteur has a large number of "standard service definitions," ofter referred to as "standard robdefs." They
can be found in the [robotraconteur_standard_robdef](https://github.com/robotraconteur/robotraconteur_standard_robdef)
GitHub repository. These standard service types include common data structures such as geometry types, image types,
device metadata structures, etc., and also include a number of standard object types that are used to represent
common devices. These common devices include articulated robots, cameras, actuators, and sensors, along with many
others. These standardized types allow for interoperability between different devices. Clients designed to use
a specific standard service type interfacewill be able to communicate with any device that provides this interface,
regardless of the model or vendor.

The standard service types can be used directly as Robot Raconteur objects and data types, or with the help
of "companion" libraries. These libraries add utilities to help work with the standard types. See the
[robotraconteur_directory](https://github.com/robotraconteur/robotraconteur-directory?tab=readme-ov-file#robot-raconteur-companion-libraries)
for information about the available companion libraries. Currently there are companion
libraries available for Python, C++, and C\#, but the companion libraries are developed separately from the core
library so check the directory for up to date information.

The "standard devices" examples demonstrate how to use robot and camera devices as clients.

These examples use the
[robotraconteur_training_sim](https://github.com/robotraconteur-contrib/robotraconteur_training_sim) "multi-robot scene"
to demonstrate using standard devices. Real devices can also be used by simply modifying the service URL to point
to the driver for the real device.

## Standard Device Examples

- [Robot](robot) - Client examples for the ABB IRB 1200 Robot and UR5e demonstrating standard robot interoperability
- [Camera](camera) - Client examples for a generic camera

## Drivers

The Robot Raconteur ecosystem contains numerous open-source drivers for various robots, sensors, and other devices. These
drivers are developed in mostly Python, C++, and C\#. These open-source drivers can be used as examples
for how to develop full-featured drivers See the
[Robot Raconteur Directory](https://github.com/robotraconteur/robotraconteur-directory) for a full
list of the various drivers available.

A short list of drivers to use as examples:

- [URRobotRaconteurDriver](https://github.com/robotraconteur-contrib/URRobotRaconteurDriver) -  A driver for UR
   robots using RTDE developed in C\#.
- [ABBRobotRaconteurDriver](https://github.com/robotraconteur-contrib/ABBRobotRaconteurDriver) - A driver for ABB
    IRC5 based robots using EGM developed in C\#.
- [robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver) - A
    camera driver for cameras using the OpenCV video capture feature developed in Python.
- [robotraconteur_joystick_driver](https://github.com/robotraconteur-contrib/robotraconteur_joystick_driver) - A
     joystick/gamepad driver developed in C++
- [tesseract_robotraconteur](https://github.com/tesseract-robotics/tesseract_robotraconteur) - A Robot Raconteur
     service for the Tesseract robot motion planner

Another widely used driver that is not currently standardized provide "motion program" control of robots. These
"motion programs" execute a list of motion commands sent to the robot, usually using the built-in controller
to execute the motions.

Motion program drivers:

- [abb_motion_program_exec](https://github.com/rpirobotics/abb_motion_program_exec) - Execute motion programs on
    ABB IRC5 based robots

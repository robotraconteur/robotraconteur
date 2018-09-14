<p align="center"><img src="https://robotraconteur.com/Themes/RobotRaconteurTheme/Content/images/RRHeader.jpg"></p>

# Robot Raconteur Core Library and Wrappers

A communication framework for robotics, automation, and the Internet of Things

http://robotraconteur.com

[Robot Raconteur version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things](https://s3.amazonaws.com/robotraconteurpublicfiles/docs/RobotRaconteur_CASE2016.pdf)

## WARNING: THIS REPOSITORY CONTAINS UNFINISHED SOFTWARE

Release Version 0.8.1 can be downloaded from http://robotraconteur.com/download. Registration is required due to US export restrictions on binary software.

## Building

The software has only been tested with Visual Studio 2017 and Boost 1.65.1. Other platforms such as Linux and Mac OSX will be supported soon. Download version 0.8.1 if these platforms are needed.

Building the core library only requires Visual Studio 2017, Boost 1.65.1, and CMake. Follow the instructions on the Boost website to build Boost. CMake uses [FindBoost](https://cmake.org/cmake/help/latest/module/FindBoost.html) to locate the boost libraries.

Python, Java, and C# use SWIG to generate intearnal language bindings. Due to a bug in SWIG, a forked repo must be used to build SWIG from source. The SWIG source can be found here: https://github.com/johnwason/swig

## Documentation

Documentation for version 0.8.1 can be found at https://robotraconteur.com/documentation . Documentation is being updated for the upcoming 0.9 release.

## License

The Robot Raconteur core library is Apache 2.0 licensed.

"Robot Raconteur" and the Robot Raconteur logo are registered trademarks of Wason Technology, LLC. All rights reserved.

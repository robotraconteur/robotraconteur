<p align="center"><img src="https://robotraconteur.com/Themes/RobotRaconteurTheme/Content/images/RRHeader.jpg"></p>

# Robot Raconteur Core Library and Wrappers

[![Build Status](https://travis-ci.org/robotraconteur/robotraconteur.svg?branch=master)](https://travis-ci.org/robotraconteur/robotraconteur)

A communication framework for robotics, automation, and the Internet of Things

[http://robotraconteur.com](http://robotraconteur.com)

[J. Wason, "Robot Raconteur® version 0.8: An Updated Communication System for Robotics, Automation, Building Control, and the Internet of Things", in Proc. IEEE Conference on Automation Science and Engineering, 2016, pp. 595-602.](https://s3.amazonaws.com/robotraconteurpublicfiles/docs/RobotRaconteur_CASE2016.pdf)

See [http://robotraconteur.com/documentation](http://robotraconteur.com/documentation) for documentation.

## Installation

### Windows

#### C++

Use `vcpkg` to install the Robot Raconteur C++ library.

```
vcpkg install robotraconteur
```

If Robot Raconteur is not found, clone [johnwason/vcpkg](https://github.com/johnwason/vcpkg).

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

### Ubuntu Xenial and Bionic

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
sudo apt-get install robotraconteur-python
sudo apt-get install robotraconteur-python3

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
brew install robotraconteur
```

If Robot Raconteur is not found, clone [johnwason/homebrew-core](https://github.com/johnwason/homebrew-core).

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

Building the core library requires Visual Studio 2012 through 2019, Boost 1.70.0, and CMake. Follow the instructions on the Boost website to build Boost. Alternatively, Boost can be built using the [vcpkg](https://github.com/Microsoft/vcpkg) utility.

### Ubuntu Bionic
Install the dependencies:

```
apt-get install default-jdk default-jdk-headless default-jre default-jre-headless python2.7-dev libpython2.7-dev libssl1.0.0 zlib1g zlib1g-dev libssl-dev libusb-1.0-0 libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev python-numpy python-setuptools python-wheel git cmake-qt-gui g++ make libboost-all-dev autoconf automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel mono-devel -qq
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
Python, Java, and C# use SWIG to generate language bindings. Robot Raconteur currently uses SWIG version 4.0.0-beta1. Earlier versions will not work. Windows binaries can be downloaded from SourceForge [swigwin-4.0.0.zip](https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.0.0/swigwin-4.0.0.zip/download) . For Mac OSX and Linux, SWIG must be build from source. Instructions can be found [here](https://github.com/swig/swig/wiki/Getting-Started).

### MATLAB Mex

MATLAB install required for build. CMake FindMatlab module is used to locate the MATLAB build dependencies. Be sure to build the MEX file against static boost libraries. Linux will require Boost to be built from source with "-fPIC" C++ gcc option. The apt repository static libraries cannot be used because they were not built with "-fPIC".  

## ROS Support

Robot Raconteur can be built and utilized from within ROS. To use, clone the robotraconteur repository into catkin_ws/src, run rosdep to install dependencies, and use catkin_make_isolated to build. By default, the resulting build will not have ROS support and will only build the core static library. To build with ROS support, RobotRaconteurGen, and Python, execute:

    catkin_make_isolated --cmake-args -DROBOTRACONTEUR_ROS=1

The additional cmake variable must be present for the first build to enable additional functionality.

Note that SWIG version 4.0.0 MUST be installed before attempting to build the Python bindings. Robot Raconteur will not build using the older versions installed from apt! Install to /usr/local so catkin can find the swig executable. See [here](https://github.com/swig/swig/wiki/Getting-Started) for SWIG installation instructions.
    
## Documentation

Documentation for version 0.8.1 can be found at [https://robotraconteur.com/documentation](https://robotraconteur.com/documentation). Documentation is being updated for the upcoming 0.9 release.

## License

The Robot Raconteur core library is Apache 2.0 licensed.

"Robot Raconteur" and the Robot Raconteur logo are registered trademarks of Wason Technology, LLC. All rights reserved.

Robot Raconteur is patent pending.

Robot Raconteur is developed by Dr. John Wason, Wason Technology, LLC

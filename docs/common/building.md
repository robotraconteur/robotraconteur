## Building

  - [Building](#building)
    - [Windows](#windows)
    - [Ubuntu Focal](#ubuntu-focal)
    - [Ubuntu Bionic](#ubuntu-bionic)
    - [Mac OSX](#mac-osx)
    - [Swig](#swig)
    - [MATLAB Mex](#matlab-mex)
    - [iOS](#ios)


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

### iOS

See https://github.com/robotraconteur/robotraconteur/wiki/iOS

### Android

See https://github.com/robotraconteur/robotraconteur/wiki/Android

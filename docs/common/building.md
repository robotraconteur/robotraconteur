# Building

- [CMake Options](#cmake-options)
- [Windows](#windows)
- [Ubuntu Focal, Jammy, and Noble](#ubuntu-focal-jammy-and-noble)
- [Ubuntu Bionic](#ubuntu-bionic)
- [Mac OSX](#mac-osx)
- [Swig](#swig)
- [MATLAB Mex](#matlab-mex)
- [iOS](#ios)

The software is tested using GitHub Actions continuous integration. See .github/workflows/main.yml for more examples
building Robot Raconteur.

## CMake Options

CMake options can be set to control the build. The following options are available:

- `BUILD_CORE`: Build the Robot Raconteur core library. If not set, the package `RobotRaconteur` will be searched
   using the CMake `find_package` command.
- `BUILD_GEN`: Build the code generator. This is required for building the Robot Raconteur code generator.
- `BUILD_TESTING`: Build the Robot Raconteur tests. This is useful for testing the
- `BUILD_SHARED_LIBS`: Build shared libraries instead of static libraries. This is the default.
- `BUILD_PYTHON3`: Build the Python 3 bindings.
- `BUILD_JAVA`: Build the Java bindings.
- `BUILD_NET`: Build the C# bindings.
- `BUILD_JAVA`: Build the C# bindings.
- `BUILD_MATLAB_MEX`: Build the MATLAB MEX bindings.
- `BUILD_DOCUMENTATION`: Build the documentation. Requires Doxygen, Sphinx, and docfx to be installed depending
  on the enabled languages.
- `ROBOTRACONTEUR_ROS`: Enable ROS mode, building the core library and Python3
- `USE_PREGENERATED_SOURCE`: Use pregenerated source instead of generating SWIG at build time.
- `BUILD_PREGENERATED_SOURCE`: Build the pregenerated source files. This is required if `USE_PREGENERATED_SOURCE` is set.
- `PREGENERATED_SOURCE_DIR`: Directory containing the pregenerated source files. Defaults to `generated_src/`.
- `ROBOTRACONTEUR_SOVERSION_MAJOR_ONLY`: Only include the major version in the shared library name.
- `RR_NET_BUILD_NATIVE_ONLY`: Build only the .NET native library, not the managed library. Use with the nuget package
  if the native library is not included in the nuget package.
- `RR_NET_INSTALL_NATIVE_LIB`: Install the .NET native library to the system.
- `INSTALL_PYTHON3`: Install the Python 3 bindings to the system.


Standard CMake options such as `CMAKE_BUILD_TYPE` and `CMAKE_INSTALL_PREFIX` are also supported.

## Windows

Robot Raconteur can be built with any Visual Studio compiler version greater than 2015. It is recommended that vcpkg
be used to install the dependencies:

```cmd
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install --triplet x64-windows boost-algorithm boost-array boost-asio boost-assign boost-atomic boost-bind boost-config boost-container boost-date-time boost-smart-ptr boost-filesystem boost-foreach boost-format boost-function boost-interprocess boost-intrusive boost-lexical-cast boost-locale boost-random boost-range boost-regex boost-scope-exit boost-signals2 boost-thread boost-tuple boost-unordered boost-utility boost-uuid boost-program-options gtest openssl
```

Adjust the triplet as necessary. For instance, use `x64-windows-static-md` for a static build with multithreaded DLL runtime.
(This is the most common configuration for static builds on Windows.)

To build Robot Raconteur, use CMake to configure the project in a Visual Studio command prompt:

```cmd
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release -DBUILD_GEN=ON -DBUILD_TESTING=ON -DBUILD_SHARED_LIBS=ON
```

Adjust the path to the vcpkg toolchain as necessary and the triplet to match your configuration. Change Release to Debug for a debug build.

Then build using:

```cmd
cmake --build build --config Release
```

The `robotraconteur` and `robotraconteur-companion` packages are included in the vcpkg registry, so you can install them directly using:

```cmd
vcpkg install robotraconteur robotraconteur-companion
```

## Ubuntu Jammy and Noble
Install the dependencies:

```bash
apt-get install default-jdk default-jdk-headless default-jre default-jre-headless zlib1g zlib1g-dev libssl-dev libusb-1.0-0 libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev git cmake g++ make libboost-all-dev autoconf automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel mono-devel libgtest-dev
```

To build:

```bash
mkdir build && cd build
cmake -DBUILD_GEN=ON -DBUILD_TESTING=ON -DBoost_USE_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON ..
make
```

## Mac OSX

Robot Raconteur requires XCode, CMake, Boost Libraries, and OpenSSL. Note that the XCode command line tools must be installed.

Use homebrew to install boost and openssl:

```sh
brew install swig boost openssl googletest icu4c
```

To build Robot Raconteur, use CMake to configure the project:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GEN=ON -DBUILD_TESTING=ON -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@3 -DOPENSSL_LIBRARIES=/usr/local/opt/openssl@3/lib
```

Then build using:

```sh
cmake --build build --config Release
```

vcpkg can also be used to install dependencies on Mac OSX. See the Windows section for instructions, adjusting the triplet to `x64-osx` or `arm64-osx` as necessary.


The `robotraconteur` and `robotraconteur-companion` packages are included in the vcpkg registry, so you can install them directly using:

```cmd
vcpkg install robotraconteur robotraconteur-companion
```

Note that vcpkg does not use brew dependencies and build all dependencies from source and will not be compatible with the brew-installed libraries.

## Swig

Python, Java, and C# use SWIG to generate language bindings. Robot Raconteur currently requires SWIG version 4.0.2 or higher. Windows
can download from http://swig.org, or install from chocolatey using

```cmd
choco install swig
```

Mac OSX can install using

```sh
brew install swig
```

Ubuntu Focal and newer can install using

```bash
sudo apt-get install swig
```

## MATLAB Mex

MATLAB install required for build. CMake FindMatlab module is used to locate the MATLAB build dependencies. Be sure to build the MEX file against static boost libraries. Linux will require Boost to be built from source with "-fPIC" C++ gcc option. The apt repository static libraries cannot be used because they were not built with "-fPIC".

## iOS

See https://github.com/robotraconteur/robotraconteur/wiki/iOS

## Android

See https://github.com/robotraconteur/robotraconteur/wiki/Android

# C++ Core Library Introduction and Getting Started {#cpp_introduction}

The flagship implementation of Robot Raconteur is the Robot Raconteur Core library. It is open-source using the Apache 2.0 license. The source code can be found at [https://github.com/robotraconteur/robotraconteur]. This section discusses the C++ implentation of the Robot Raconteur framework. The "Framework" section should be read before this section (See \ref introduction).

The Robot Raconteur Core library provides a C++ API, and "wrappers" for other languages including Python, Java, C\#, and MATLAB. **C++ programming is an advanced topic, and not recommended for novice programmers. Use of one of the wrapped languages is recommended.**

The C++ library uses the [Boost C++ Libraries](https://www.boost.org/), in particular the [Boost.Asio](https://www.boost.org/doc/libs/1_72_0/doc/html/boost_asio.html) networking and communication library. The C++ library is multithreaded and asynchronous. Multithreading means that multiple threads can safely interact with the core library. Asynchronous means that functions can be invoked, with the result returned later in a callback instead of the thread waiting for completion of the operation. Multithreading and asynchronous programming is enabled through the use of the boost::asio::io_context class. This class implements network event handling and thread dispatching. See \ref threading for more details on using threading.

Robot Raconteur Core uses [CMake](https://cmake.org) for its build system. CMake files are provided for use with `find_package()`. It is recommended that projects use CMake with Robot Raconteur.

The Robot Raconteur Core library has been designed to have very few dependencies. These dependencies are typically limited to [Boost C++ Libraryis](https://boost.org), [OpenSSL](https://openssl.org) on non-Windows platforms, and a handful of device-driver related packages on Linux when the HardwareTransport is built. (These device-driver related packages are not required at runtime unless the HardwareTransport is being used.) Having a limited number of dependencies makes Robot Raconteur highly portable and easy to embed in other programs as a plugin.

Robot Raconteur Core is written using the C++98 standard. This choice was made to ease porting to industrial operating systems that may not support C++11. C++11 convenience template aliases are provided for many types when C++11 is available, and their use is recommended.

Robot Raconteur Core is distributed as a static library. While it can be built as a shared library, this is not recommended since the generated wrappers must perfectly match the Robot Raconteur library version. Statically linking the Robot Raconteur library guarantees that they will match.

## Installation {#cpp_installation}

This section only covers C++ library installation. See the project [README](https://github.com/robotraconteur/robotraconteur/blob/master/README.md) for other languages and the most up to date instructions.

Use of these installation methods is recommended since they will automatically install dependencies.

### Windows

`vcpkg` is used to install the Robot Raconteur C++ library. See https://github.com/microsoft/vcpkg for installation instructions.

Note that once built, the `vcpkg` toolchain file must be specified using the `CMAKE_TOOLCHAIN_FILE` CMake variable. See https://github.com/microsoft/vcpkg#quick-start-windows for a quick-start on using `vcpkg` with your project.
The `robotraconteur` package is available in the vcpkg registry.

```
vcpkg install robotraconteur
```

To build x64, use:

```
vcpkg --triplet=x64-windows install robotraconteur
```

### Ubuntu Xenial, Bionic, and Focal

A PPA is available for Robot Raconteur. [https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa](https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa)

```
sudo add-apt-repository ppa:robotraconteur/ppa -y
sudo apt-get update
sudo apt-get install robotraconteur-dev
```

The Robot Raconteur package will be immediately available in the CMake search path.

### Mac OSX

Use `brew` to install the Robot Raconteur C++ library.

```
brew install robotraconteur/robotraconteur/robotraconteur
```

The Robot Raconteur package will be immediately available in the CMake search path.

### Other Platforms

Other platforms must be built from source.

## Building {#cpp_building}

This section only covers building the C++ library. See the project [README](https://github.com/robotraconteur/robotraconteur/blob/master/README.md) for other languages and the most up to date instructions.

Building the C++ library is straight forward and unlikely to cause errors, however building the wrappers for other languages can be challenging. It is recommended that the `.github/workflows/main.yml` GitHub Action be used to build the wrappers.

When building using CMake, the "generator" and "build configuration" can be specified. See [CMake Generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) and CMake [CMake Build Tool Mode](https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-tool-mode) for more information. Valid build configurations are Debug, Release, RelWithDebInfo, and MinSizeRel.

### Windows

Building the core library requires Visual Studio 2012 through 2019, Boost 1.72.0, and CMake. Follow the instructions on the Boost website to build Boost. Alternatively, Boost can be built using the [vcpkg](https://github.com/Microsoft/vcpkg) utility.

If boost was built using `vcpkg`, use the following commands to build:

    mkdir build && cd build
    cmake -G <generator> .. -DCMAKE_TOOLCHAIN_FILE=<vcpkg_toolchain_path> -DBUILD_GEN=ON
    cmake --build . -C Debug
    cmake --build . --target install

### Linux

Building on linux requires several packages to be installed. For a Debian based system, the following will install the required dependencies:

    sudo apt install zlib1g-dev libssl-dev libdbus-1-dev libbluetooth-dev cmake g++ build-essential libboost-all-dev libusb-1.0-0-dev

To build the library:

    mkdir build && cd build
    cmake -DBUILD_GEN=ON ..
    cmake --build . -C Debug
    cmake --build . --target install

### Mac OSX

Building on Mac OSX requires XCode and the XCode Developer Tools to be installed. See [Apple Technical Note TN2339](https://developer.apple.com/library/archive/technotes/tn2339/_index.html) It also requires Homebrew to be installed.

Install required dependencies:

    brew install boost openssl cmake

To build the library:

    mkdir build && cd build
    cmake -G "Xcode" -DBUILD_GEN=ON -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_USE_STATIC_LIBS=ON ..
    cmake --build . _C Debug
    cmake --build . --target install

Note that Mac OSX has a version of OpenSSL installed that is incompatible with new software being built. It is critical that `OPENSSL_ROOT_DIR` and `OPENSSL_USE_STATIC_LIBS` be used to prevent collision with the default OpenSSL version.

## ROS Support {#cpp_ros}

Robot Raconteur can be built and utilized from within ROS. To use, clone the robotraconteur repository into catkin_ws/src, run rosdep to install dependencies, and use catkin_make_isolated or catkin_tools to build. By default, the resulting build will not have ROS support and will only build the core static library. To build with ROS support, RobotRaconteurGen, and Python, the CMake option `ROBOTRACONTEUR_ROS` must be set.

Before building, run `rosdep`:

    rosdep install --from-paths src --ignore-src -r -y

To build with `catkin_make_isolated`:

    catkin_make_isolated --cmake-args -DROBOTRACONTEUR_ROS=1

To build with `catkin_tools`:

    catkin config --cmake-args -DROBOTRACONTEUR_ROS=1
    catkin build

Note that SWIG version 4.0.0 MUST be installed before attempting to build the Python bindings. Ubuntu 20.04 Focal includes SWIG 4.0.0, but older versions of Ubuntu do not. Robot Raconteur will not build using theese older versions installed from apt! For these older Ubuntu versions, SWIG must be built from source Install to /usr/local so catkin can find the swig executable. See [here](https://github.com/swig/swig/wiki/Getting-Started) for SWIG installation instructions.

## CMake and Robot Raconteur Thunk Source Generation {#cpp_intro_cmake_thunk_source_gen}

Using Robot Raconteur from CMake is straightforward. In your CMakeLists.txt file, use `find_package` to find Robot Raconteur:

    find_package(RobotRaconteur REQUIRED)

Robot Raconteur uses Service Definitions to define objects, value types, exceptions, constants, and enums. For dynamic languages like Python and MATLAB, these types are handled automatically. Compiled languages like C++ need to have these types generated at compile time. This is accomplished using the `RobotRaconteurGen` utility. See \ref robotraconteurgen for more information on this utility. Robot Raconteur provides a CMake macro to call `RobotRaconteurGen`.

    ROBOTRACONTEUR_GENERATE_THUNK(<SRCS> <HDRS> <ROBDEF_FILES> [MASTER_HEADER] [AUTO_IMPORT] [INCLUDE_DIRS <INCLUDE_DIR1> [<INCLUDE_DIR2> ...]])

See \ref robotraconteur_generate_thunk for more details on using the `ROBOTRACONTEUR_GENERATE_THUNK` macro.

Add the `<src_files_out>` variable to your input sources in the `add_executable()` or `add_library()` command.

For your targets, add the RobotRaconteurCore target as a dependency. This will automatically add the required include directories and required libraries.

    target_link_libraries(my_target RobotRaconteurCore)

See the C++ examples for example CMakeLists.txt files.

If the `MASTER_HEADER` header is used, use the following include in program header files:

    #include <RobotRaconteur.h>
    #include "robotraconteur_generated.h"

## Boost Library {#cpp_boost_library}

The boost libraries are used extensively throughout Robot Raconteur  The following classes are of particular interest by software using Robot Raconteur:

* `boost::shared_ptr<T>`
* `boost::make_shared<T>`
* `boost::enable_shared_from_this<T>`
* `boost::function<...>`
* `boost::bind<...>`
* `boost::signals2::signal<...>`
* `boost::thread`
* `boost::recursive_mutex`
* `boost::posix_time::ptime`

Documentation for these types can be found in the [Boost documentation](https://www.boost.org/doc/libs/).

Robot Raconteur uses a number of preprocessor defines for Boost and C++11 types. These are all defined in `RobotRaconteurConfig.h` These are used to allow for configurability. By default, the following defines are used:

    #define RR_SHARED_PTR std::shared_ptr
    #define RR_MAKE_SHARED std::make_shared
    #define RR_WEAK_PTR std::weak_ptr
    #define RR_ENABLE_SHARED_FROM_THIS std::enable_shared_from_this
    #define RR_DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
    #define RR_STATIC_POINTER_CAST std::static_pointer_cast
    #define RR_INTRUSIVE_PTR boost::intrusive_ptr
    #define RR_UNORDERED_MAP boost::unordered_map

There are a number of defines that are used for incompatibilities between Boost and C++ versions. Consult `RobotRaconteurConfig.h` for there definitions, since the definitions are configuration dependent.

    RR_MOVE_ARG
    RR_MOVE
    RR_BOOST_ASIO_IO_CONTEXT
    RR_BOOST_ASIO_STRAND
    RR_BOOST_ASIO_POST(context, func)
    RR_BOOST_ASIO_BUFFER_CAST(type,buf)
    RR_BOOST_ASIO_STRAND_WRAP(strand, f)
    RR_BOOST_ASIO_NEW_STRAND(context)
    RR_BOOST_PLACEHOLDERS

## Smart Pointers and Aliases {#cpp_smart_pointers}

Robot Raconteur uses smart pointers for memory management. Smart pointers use "reference counting" to track the number of currently active references to an object or data. When the number of references go to zero, the object is deleted.

Two types of smart pointers are used: standard smart pointers, implemented using `boost::shared_ptr<T>`, and intrusive pointers, implemented using `boost::intrusive_ptr<T>`. The difference between these pointers is how the reference count is stored in memory. `boost::shared_ptr<T>` allocates a small amount of separate memory on the heap using `new` to store the reference count. This has the advantage of not affecting the object itself, but has the overhead of allocting and destroying this extra memory segment. `boost::intrusive_ptr` requires the object or structure to have a field specifically for reference counting declared in the object/structure itself.

Robot Raconteur uses `boost::shared_ptr<T>` for API objects, service objects, and service object references (proxies). `boost::intrusive_ptr<T>` is used for all value types. Value types inherit from RobotRaconteur::RRValue, which contains the field for reference counting.

The preprocessor macros `RR_SHARED_PTR` and `RR_INTRUSIVE_PTR` are used internally for shared and intrusive pointer types. These are simply aliases to `boost::shared_ptr<T>` and `boost::intrusive_ptr<T>`.

For C++11 capable compilers, "template aliases" are available to make the code look cleaner. API types and generated types have these aliases declared. (RobotRaconteurGen will generate the aliases). A few (pseudocode) examples of aliases and their expansion:

    RobotRaconteurNodePtr = boost::shared_ptr<RobotRaconteurNode>
    RRArrayPtr<int> = boost::intrusive_ptr<RRArray<int> >
    MyObjectPtr = boost::shared_ptr<MyObject> // Service definition generated object
    MyStructPtr = boost::intrusive_ptr<MyObject> // Service definition generated struct

## Boost string_ref

`boost::string_ref` is used for string parameters in the Robot Raconteur library. The `boost::string_ref` is used instead of `std::string` to prevent runtime copies. It can be initialized from a `std::string`, a `char*` and length, or a string literal. It can be converted back to a string using `boost::string_ref::to_string()`. The conversions are normally done automatically, so the user does not need to do anything different compared to using normal `std::string` parameters.

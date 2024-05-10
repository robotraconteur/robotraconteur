# Reynard C++ Robot Raconteur Client Example

This example demonstrates controlling Reynard using C++ as a Robot Raconteur client.

This example requires installing the appropriate C++ libraries based on the operating system. See
https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md for more information.
On Windows, it is assumed `vcpkg` has been set up in `C:\ws\vcpkg`. The `robotraconteur` package must be installed
using the `x64-windows` triplet. On Ubuntu the `apt` packages are expected. On Mac OS `brew` is expected.

## Compiling

On Windows, run:

```
cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=C:\ws\vcpkg_with_rr\scripts\buildsystems\vcpkg.cmake -S. -B build
cmake --build build
```

On Ubuntu and Mac OS, run:


```
cmake -G"Unix Makefiles" -S. -B build
cmake --build build
```

## Run Example

On Windows, run:

```
build\reynard_robotraconteur_client.exe
```

On Ubuntu and Mac OS, run:

```
./build/reynard_robotraconteur_client.exe
```

# Config options for ROS 1 and 2

set(ROBOTRACONTEUR_ROS ON CACHE BOOL "Enable ROS support")
set(USE_PREGENERATED_SOURCE OFF CACHE BOOL "Use pregenerated source files")
set(ROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK ON)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
set(ROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY ON CACHE BOOL "Only include major version in SOVERSION")
# The EL10 rpmbuild RPATH check rejects the build-tree RPATH that setup.py copies into the Python module. Build without
# RPATH.
set(ROBOTRACONTEUR_SKIP_RPATH ON CACHE BOOL "Skip RPATH for shared libraries")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

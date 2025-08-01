# Config options for ROS 1 and 2

set(ROBOTRACONTEUR_ROS ON CACHE BOOL "Enable ROS support")
set(USE_PREGENERATED_SOURCE ON CACHE BOOL "Use pregenerated source files")
set(ROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK ON)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
set(ROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY ON CACHE BOOL "Only include major version in SOVERSION")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

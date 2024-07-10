cmake_minimum_required(VERSION 3.3)
project(camera_client_examples)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

find_package(RobotRaconteurCompanion REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

if(MSVC)
    # Silence warning C4251 about dll-interface for std::vector, etc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4250")
endif()

# This example only uses standard types from the Companion library so no generation is required

add_executable(camera_client_capture_frame camera_client_capture_frame.cpp)
target_link_libraries(camera_client_capture_frame RobotRaconteurCompanion opencv_core opencv_highgui RobotRaconteurCore)

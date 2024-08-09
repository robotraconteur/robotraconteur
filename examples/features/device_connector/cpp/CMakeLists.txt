cmake_minimum_required(VERSION 3.3)
project(device_connector)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

find_package(RobotRaconteurCompanion REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(device_connector device_connector.cpp)
target_link_libraries(device_connector RobotRaconteurCompanion opencv_core opencv_highgui RobotRaconteurCore)

add_executable(device_connector_yaml device_connector_yaml.cpp)
target_link_libraries(device_connector_yaml RobotRaconteurCompanion opencv_core opencv_highgui RobotRaconteurCore)

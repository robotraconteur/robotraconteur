cmake_minimum_required(VERSION 3.3)
project(robot_client_examples)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

find_package(RobotRaconteurCompanion REQUIRED)
find_package(Eigen3 REQUIRED)

if(MSVC)
    # Silence warning C4251 about dll-interface for std::vector, etc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4250")
endif()

# This example only uses standard types from the Companion library so no generation is required

add_executable(robot_client_jog_freespace robot_client_jog_freespace.cpp)
target_link_libraries(robot_client_jog_freespace RobotRaconteurCompanion RobotRaconteurCore)

add_executable(robot_client_position_command robot_client_position_command.cpp)
target_link_libraries(robot_client_position_command RobotRaconteurCompanion RobotRaconteurCore)

add_executable(robot_client_trajectory robot_client_trajectory.cpp)
target_link_libraries(robot_client_trajectory RobotRaconteurCompanion RobotRaconteurCore)

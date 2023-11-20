# Minimal CMakeLists.txt to build disttest_cpp.cpp using RobotRaconteur package

cmake_minimum_required(VERSION 3.5)
project(disttest_cpp)

find_package(RobotRaconteur REQUIRED)

add_executable(disttest_cpp disttest_cpp.cpp)
target_link_libraries(disttest_cpp RobotRaconteurCore)

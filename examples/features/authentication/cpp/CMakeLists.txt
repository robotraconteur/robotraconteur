cmake_minimum_required(VERSION 3.3)
project(authentication_example)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.reynard_the_robot.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../reynard_the_robot/robdef
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(authentication_example authentication_example.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(authentication_example RobotRaconteurCore)

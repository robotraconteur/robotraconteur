cmake_minimum_required(VERSION 3.3)
project(exception_example)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.exception_example.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../robdef
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(exceptions_example exceptions_example.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(exceptions_example RobotRaconteurCore)

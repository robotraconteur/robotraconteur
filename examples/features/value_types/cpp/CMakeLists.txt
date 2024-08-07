cmake_minimum_required(VERSION 3.3)
project(value_types)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.value_types.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../robdef
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(numeric_value_types numeric_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(numeric_value_types RobotRaconteurCore)

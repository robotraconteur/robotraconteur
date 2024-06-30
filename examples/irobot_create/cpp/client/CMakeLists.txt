cmake_minimum_required(VERSION 3.3)
project(create_client)

set(CMAKE_CXX_STANDARD 11)

find_package(RobotRaconteur REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

robotraconteur_generate_thunk(RR_THUNK_SRCS RR_THUNK_HDRS experimental.create3.robdef MASTER_HEADER INCLUDE_DIRS
                              ${CMAKE_CURRENT_SOURCE_DIR}/../../robdef)

add_executable(irobot_create_client ${RR_THUNK_HDRS} ${RR_THUNK_SRCS} irobot_create_client.cpp)

target_link_libraries(irobot_create_client RobotRaconteurCore)

cmake_minimum_required(VERSION 3.3)
project(web_https_client)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.wstest.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../robdef
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(web_https_client web_https_client.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})

target_link_libraries(web_https_client RobotRaconteurCore)

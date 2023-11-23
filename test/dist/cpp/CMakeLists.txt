# Minimal CMakeLists.txt to build disttest_cpp.cpp using RobotRaconteur package

cmake_minimum_required(VERSION 3.5)
project(disttest_cpp)

find_package(RobotRaconteur REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ROBOTRACONTEUR_GENERATE_THUNK(RR_THUNK_SRCS RR_THUNK_HDRS
    experimental.distfiles_test.robdef
	MASTER_HEADER
	)

add_executable(disttest_cpp disttest_cpp.cpp ${RR_THUNK_HDRS} ${RR_THUNK_SRCS})
target_link_libraries(disttest_cpp RobotRaconteurCore)

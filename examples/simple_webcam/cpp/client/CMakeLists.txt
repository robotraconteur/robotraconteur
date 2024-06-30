cmake_minimum_required(VERSION 3.3)
project(simple_webcam_client)

set(CMAKE_CXX_STANDARD 11)

find_package(RobotRaconteur REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

robotraconteur_generate_thunk(RR_THUNK_SRCS RR_THUNK_HDRS experimental.simplewebcam3.robdef MASTER_HEADER INCLUDE_DIRS
                              ${CMAKE_CURRENT_SOURCE_DIR}/../../robdef)

add_executable(simple_webcam_client ${RR_THUNK_HDRS} ${RR_THUNK_SRCS} simple_webcam_client.cpp)
target_link_libraries(simple_webcam_client opencv_core opencv_highgui RobotRaconteurCore)

add_executable(simple_webcam_client_memory ${RR_THUNK_HDRS} ${RR_THUNK_SRCS} simple_webcam_client_memory.cpp)
target_link_libraries(simple_webcam_client_memory opencv_core opencv_highgui RobotRaconteurCore)

add_executable(simple_webcam_client_streaming ${RR_THUNK_HDRS} ${RR_THUNK_SRCS} simple_webcam_client_streaming.cpp)
target_link_libraries(simple_webcam_client_streaming opencv_core opencv_highgui RobotRaconteurCore)

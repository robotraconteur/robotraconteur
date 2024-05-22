cmake_minimum_required(VERSION 3.3)
project(reynard_robotraconteur_service)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL "")
find_package(
    Boost
    COMPONENTS date_time
               filesystem
               system
               regex
               chrono
               atomic
               thread
               random
               program_options
    REQUIRED)
find_package(drekar-launch-process-cpp CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.reynard_the_robot.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../robdef/
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(reynard_robotraconteur_service_http_rest reynard_robotraconteur_service_http_rest.cpp ${RR_THUNK_SRCS}
                                                        ${RR_THUNK_HDRS})
target_link_libraries(reynard_robotraconteur_service_http_rest RobotRaconteurCore drekar-launch-process-cpp
                      nlohmann_json::nlohmann_json)

add_executable(reynard_robotraconteur_service_ascii_socket reynard_robotraconteur_service_ascii_socket.cpp
                                                           ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(reynard_robotraconteur_service_ascii_socket RobotRaconteurCore drekar-launch-process-cpp)

# Master cmake file to include examples in main build.
#
# This file is not relevant to using the examples in your project. It is only used to build the examples as part of the
# main project build.

cmake_minimum_required(VERSION 3.3)
project(robotraconteur_examples)

if(NOT ROBOTRACONTEUR_GENERATE_THUNK)
    include(${CMAKE_CURRENT_LIST_DIR}/../RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)
endif()
add_subdirectory(reynard_the_robot/cpp/client)
add_subdirectory(reynard_the_robot/cpp/service)

option(EXAMPLES_BUILD_NET "Build .NET examples" ON)
if(EXAMPLES_BUILD_NET)

    # Add c# build commands
    add_custom_target(reynard_the_robot_csharp_client ALL COMMAND dotnet build reynard_robotraconteur_client.csproj
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/reynard_the_robot/cs/client)
endif()

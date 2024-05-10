# Master cmake file to include examples in main build.
#
# This file is not relevant to using the examples in your project. It is only used to build the examples as part of the
# main project build.

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
    if(NOT ROBOTRACONTEUR_GENERATE_THUNK)
        include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)
    endif()
    add_subdirectory(reynard_the_robot/cpp/client)
endif()

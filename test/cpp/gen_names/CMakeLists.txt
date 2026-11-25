if(NOT ROBOTRACONTEUR_GENERATE_THUNK)
    include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)
endif()

# One test per experimental.gen_test_cpp_<name>.robdef, with its test in gen_<name>_test.cpp: each checks that the C++
# thunk generated for members with problematic names compiles and works CONFIGURE_DEPENDS (CMake 3.12+) re-runs the glob
# when robdefs are added, e.g. after merging another of these tests
set(RR_GEN_NAMES_GLOB_ARGS "")
if(NOT CMAKE_VERSION VERSION_LESS 3.12)
    set(RR_GEN_NAMES_GLOB_ARGS CONFIGURE_DEPENDS)
endif()
file(GLOB RR_GEN_NAMES_ROBDEFS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" ${RR_GEN_NAMES_GLOB_ARGS}
                                                                      "${CMAKE_CURRENT_SOURCE_DIR}/*.robdef")
foreach(RR_GEN_NAMES_ROBDEF ${RR_GEN_NAMES_ROBDEFS})
    string(REGEX REPLACE "^experimental\\.gen_test_cpp_(.*)\\.robdef$" "\\1" RR_GEN_NAMES_NAME "${RR_GEN_NAMES_ROBDEF}")
    robotraconteur_generate_thunk(RR_GEN_NAMES_${RR_GEN_NAMES_NAME}_SRCS RR_GEN_NAMES_${RR_GEN_NAMES_NAME}_HDRS
                                  ${RR_GEN_NAMES_ROBDEF})
    rr_test_add_executable(
        robotraconteur_test_gen_${RR_GEN_NAMES_NAME}
        SRC
        gen_${RR_GEN_NAMES_NAME}_test.cpp
        ${RR_GEN_NAMES_${RR_GEN_NAMES_NAME}_SRCS}
        INCLUDE
        ${CMAKE_CURRENT_BINARY_DIR}
        DEPS
        RobotRaconteurCore
        GTest::Main)
endforeach()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake)

find_package(GTest)

add_subdirectory(robdef)
add_subdirectory(lfsr)

if(BUILD_CORE)
    add_subdirectory(cpp)
endif()

if(BUILD_PYTHON OR BUILD_PYTHON3)
    add_subdirectory(python)
endif()

if(BUILD_NET)
    add_subdirectory(net)
endif()

if(BUILD_JAVA)
    add_subdirectory(java)
endif()

if(BUILD_MATLAB_MEX)
    add_subdirectory(matlab)
endif()

option(ROBOTRACONTEUR_EXTRA_TESTS "Run extra tests" OFF)

if(ROBOTRACONTEUR_EXTRA_TESTS)
    add_subdirectory(extra)
endif()

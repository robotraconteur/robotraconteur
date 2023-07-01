cmake_minimum_required(VERSION 3.3)

# Read version

file(STRINGS "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h"
     ROBOTRACONTEUR_CONFIG_VERSION REGEX "#define +ROBOTRACONTEUR_VERSION_TEXT +\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
if("${ROBOTRACONTEUR_CONFIG_VERSION}" STREQUAL "")
    message(FATAL_ERROR "Could not read Robot Raconteur version.")
endif()
string(REGEX REPLACE "#define ROBOTRACONTEUR_VERSION_TEXT \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" "\\1" ROBOTRACONTEUR_VERSION
                     ${ROBOTRACONTEUR_CONFIG_VERSION})

project(RobotRaconteur VERSION ${ROBOTRACONTEUR_VERSION})

message(STATUS "Robot Raconteur version: ${ROBOTRACONTEUR_VERSION}")

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/cmake_config_options.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake_config_options.cmake)
endif()

include(cmake/CMakeMacros.cmake)
include(CTest)

include(CheckTypeSize)
check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)

configure_file("${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h"
               "${CMAKE_BINARY_DIR}/RobotRaconteurConfig.h" COPYONLY)

if(NOT ROBOTRACONTEUR_VERSION_SEMVER)
    set(ROBOTRACONTEUR_VERSION_SEMVER ${ROBOTRACONTEUR_VERSION})
endif()

string(REGEX REPLACE "^([0-9]+\\.[0-9]+\\.[0-9]+)" "\\1" ROBOTRACONTEUR_VERSION_S ${ROBOTRACONTEUR_VERSION_SEMVER})
if(NOT ${ROBOTRACONTEUR_VERSION} VERSION_EQUAL ${ROBOTRACONTEUR_VERSION_S})
    message(
        FATAL_ERROR
            "Supplied semver ${ROBOTRACONTEUR_VERSION_SEMVER} does not equal config version ${ROBOTRACONTEUR_VERSION}")
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/rrversion.txt.in ${CMAKE_BINARY_DIR}/rrversion.txt @ONLY)

include(GNUInstallDirs)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build documentation using Doxygen" ${DOXYGEN_FOUND})

if(APPLE)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(BUILD_CORE ON CACHE BOOL "Build core library")
if(BUILD_CORE)
    add_subdirectory(RobotRaconteurCore)
    set(RobotRaconteur_CORE_LIBRARY RobotRaconteurCore)
else()
    find_package(RobotRaconteur REQUIRED)
    set(RobotRaconteur_CORE_LIBRARY RobotRaconteurCore)
endif()

set(ROBOTRACONTEUR_ROS "" CACHE BOOL "Build Robot Raconteur for ROS workspace")
if(ROBOTRACONTEUR_ROS)
    set(BUILD_GEN ON CACHE BOOL "Build RobotRaconteurGen")
    set(BUILD_TESTING ON CACHE BOOL "Build RobotRaconteurTest")
    set(BUILD_PYTHON3 ON CACHE BOOL "Build Python 3 wrappers")
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/generated_src)
    set(USE_PREGENERATED_SOURCE ON CACHE BOOL ON)
else()
    set(USE_PREGENERATED_SOURCE OFF CACHE BOOL OFF)
endif()

if(BUILD_PYTHON
   OR BUILD_PYTHON3
   OR BUILD_NET
   OR BUILD_JAVA)
    if(USE_PREGENERATED_SOURCE)
        set(PREGENERATED_SOURCE_DIR ${CMAKE_SOURCE_DIR}/generated_src CACHE PATH "generated_src directory location")
        if(NOT EXISTS ${PREGENERATED_SOURCE_DIR}/Python/swigwordsize32/RobotRaconteurPythonPYTHON_wrap.cxx
           OR NOT EXISTS ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize32/RobotRaconteurPythonPYTHON_wrap.cxx
           OR NOT EXISTS
              ${PREGENERATED_SOURCE_DIR}/Java/swigwordsize32/com/robotraconteur/RobotRaconteurJavaJAVA_wrap.cxx
           OR NOT EXISTS ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize32/RobotRaconteurNETCSHARP_wrap.cxx)
            message(FATAL_ERROR "Invalid PREGENERATED_SOURCE_DIR")
        endif()
    endif()
endif()

set(BUILD_GEN ON CACHE BOOL "Build RobotRaconteurGen")
if(BUILD_GEN)
    add_subdirectory(RobotRaconteurGen)
endif()

set(BUILD_PYTHON "" CACHE BOOL "Build Python wrappers")
if(BUILD_PYTHON)
    add_subdirectory(RobotRaconteurPython)
endif()

set(BUILD_PYTHON3 "" CACHE BOOL "Build Python 3.x wrappers")
if(BUILD_PYTHON3)
    add_subdirectory(RobotRaconteurPython3)
endif()

set(BUILD_NET "" CACHE BOOL "Build .NET wrappers")
if(BUILD_NET)
    add_subdirectory(RobotRaconteurNET)
endif()

set(BUILD_JAVA "" CACHE BOOL "Build Java wrappers")
if(BUILD_JAVA)
    add_subdirectory(RobotRaconteurJava)
endif()

set(BUILD_MATLAB_MEX OFF CACHE BOOL OFF)
if(BUILD_MATLAB_MEX)
    add_subdirectory(RobotRaconteurMex)
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

if(BUILD_PYTHON
   OR BUILD_PYTHON3
   OR BUILD_NET
   OR BUILD_JAVA
   AND NOT USE_PREGENERATED_SOURCE)

    include(${CMAKE_SOURCE_DIR}/cmake/package_source_swig.cmake)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "cmake/Config.cmake.in" "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
    PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_BINDIR)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${ROBOTRACONTEUR_VERSION} COMPATIBILITY AnyNewerVersion)

install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.git/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.vs/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.vscode/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.github/")
# list(APPEND CPACK_SOURCE_IGNORE_FILES "/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "^${CMAKE_SOURCE_DIR}/build[\\\\-_/]")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.cache")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.gitignore")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.travis.yml")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.clang-format")

set(CPACK_PACKAGE_VERSION ${ROBOTRACONTEUR_VERSION_SEMVER})

set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_SOURCE_DIR};/;${CMAKE_BINARY_DIR}/generated_src;/generated_src")
set(CPACK_SOURCE_TBZ2 OFF CACHE BOOL "" FORCE)
set(CPACK_SOURCE_TXZ OFF CACHE BOOL "" FORCE)
set(CPACK_SOURCE_TZ OFF CACHE BOOL "" FORCE)
if(NOT WIN32)
    set(CPACK_SOURCE_TGZ ON CACHE BOOL "" FORCE)
endif()
set(CPACK_SOURCE_ZIP ON CACHE BOOL "" FORCE)

include(CPack)

if(ROBOTRACONTEUR_ROS OR ROBOTRACONTEUR_COLCON)
    # Allows Colcon to find non-Ament packages when using workspace underlays
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/share/ament_index/resource_index/packages/robotraconteur "")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/ament_index/resource_index/packages/robotraconteur
            DESTINATION share/ament_index/resource_index/packages)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/share/robotraconteur/hook/ament_prefix_path.dsv
         "prepend-non-duplicate;AMENT_PREFIX_PATH;")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/robotraconteur/hook/ament_prefix_path.dsv
            DESTINATION share/robotraconteur/hook)
endif()

file(STRINGS "${CMAKE_SOURCE_DIR}/package.xml" ROBOTRACONTEUR_ROS_PKG_VERSION_1
     REGEX "<version>[0-9]+\\.[0-9]+\\.[0-9]+</version>")
string(REGEX REPLACE "<version>([0-9]+\\.[0-9]+\\.[0-9]+)</version>" "\\1" ROBOTRACONTEUR_ROS_PKG_VERSION
                     ${ROBOTRACONTEUR_ROS_PKG_VERSION_1})
if(NOT "${ROBOTRACONTEUR_ROS_PKG_VERSION}" VERSION_EQUAL "{ROBOTRACONTEUR_VERSION}")
    message(
        FATAL_ERROR "Found ROS package version ${ROBOTRACONTEUR_ROS_PKG_VERSION} mismatch ${ROBOTRACONTEUR_VERSION}")
endif()

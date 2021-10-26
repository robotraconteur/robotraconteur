cmake_minimum_required(VERSION 3.3)
project(RobotRaconteur)

include(cmake/CMakeMacros.cmake)
include(CTest)

include(CheckTypeSize)
check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)

CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h" "${CMAKE_BINARY_DIR}/RobotRaconteurConfig.h" COPYONLY)

FILE (STRINGS "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h" ROBOTRACONTEUR_CONFIG_VERSION REGEX "#define +ROBOTRACONTEUR_VERSION_TEXT +\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
IF ("${ROBOTRACONTEUR_CONFIG_VERSION}" STREQUAL "" )
MESSAGE(FATAL_ERROR "Could not read Robot Raconteur version.")
ENDIF()
STRING (REGEX REPLACE "#define ROBOTRACONTEUR_VERSION_TEXT \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" "\\1" ROBOTRACONTEUR_VERSION ${ROBOTRACONTEUR_CONFIG_VERSION})
MESSAGE(STATUS "Robot Raconteur version: ${ROBOTRACONTEUR_VERSION}")

if(NOT ROBOTRACONTEUR_VERSION_SEMVER)
set(ROBOTRACONTEUR_VERSION_SEMVER ${ROBOTRACONTEUR_VERSION})
endif()

STRING (REGEX REPLACE "^([0-9]+\\.[0-9]+\\.[0-9]+)" "\\1" ROBOTRACONTEUR_VERSION_S ${ROBOTRACONTEUR_VERSION_SEMVER})
if (NOT ${ROBOTRACONTEUR_VERSION} VERSION_EQUAL ${ROBOTRACONTEUR_VERSION_S})
message(FATAL_ERROR "Supplied semver ${ROBOTRACONTEUR_VERSION_SEMVER} does not equal config version ${ROBOTRACONTEUR_VERSION}")
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/rrversion.txt.in ${CMAKE_BINARY_DIR}/rrversion.txt @ONLY)

include(GNUInstallDirs)

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Build documentation using Doxygen" ${DOXYGEN_FOUND})

if (APPLE)
set (CMAKE_CXX_STANDARD 11)
endif()

set(BUILD_CORE ON CACHE BOOL "Build core library")
if (BUILD_CORE)
add_subdirectory(RobotRaconteurCore)
set(RobotRaconteur_CORE_LIBRARY RobotRaconteurCore)
else()
find_package(RobotRaconteur REQUIRED)
set(RobotRaconteur_CORE_LIBRARY RobotRaconteurCore)
endif()

set (ROBOTRACONTEUR_ROS "" CACHE BOOL "Build Robot Raconteur for ROS workspace")
if(ROBOTRACONTEUR_ROS)
set(BUILD_GEN true CACHE BOOL "Build RobotRaconteurGen")
set(BUILD_TEST true CACHE BOOL "Build RobotRaconteurTest")
if ($ENV{ROS_PYTHON_VERSION} LESS 3)
set(BUILD_PYTHON true CACHE BOOL "Build Python wrappers")
else()
set(BUILD_PYTHON3 true CACHE BOOL "Build Python 3 wrappers")
endif()
endif()

set (BUILD_GEN "" CACHE BOOL "Build RobotRaconteurGen")
if (BUILD_GEN)
add_subdirectory(RobotRaconteurGen)
endif()

set(BUILD_TEST "" CACHE BOOL "Build RobotRaconteurTest")
if (BUILD_TEST)
add_subdirectory(testing/RobotRaconteurTest)
endif()

set(BUILD_PYTHON "" CACHE BOOL "Build Python wrappers")
if (BUILD_PYTHON)
add_subdirectory(RobotRaconteurPython)
endif()

set(BUILD_PYTHON3 "" CACHE BOOL "Build Python 3.x wrappers")
if (BUILD_PYTHON3)
add_subdirectory(RobotRaconteurPython3)
endif()

set(BUILD_NET "" CACHE BOOL "Build .NET wrappers")
if (BUILD_NET)
add_subdirectory(RobotRaconteurNET)
endif()

set(BUILD_JAVA "" CACHE BOOL "Build Java wrappers")
if (BUILD_JAVA)
add_subdirectory(RobotRaconteurJava)
endif()

set(BUILD_MATLAB_MEX OFF CACHE BOOL OFF)
IF (BUILD_MATLAB_MEX)
add_subdirectory(RobotRaconteurMex)
ENDIF()

if (EXISTS ${CMAKE_SOURCE_DIR}/generated_src)
set(USE_PREGENERATED_SOURCE ON CACHE BOOL ON)
else()
set(USE_PREGENERATED_SOURCE OFF CACHE BOOL OFF)
endif()

if(BUILD_PYTHON OR BUILD_NET OR BUILD_JAVA)
if(USE_PREGENERATED_SOURCE)	
set(PREGENERATED_SOURCE_DIR ${CMAKE_SOURCE_DIR}/generated_src CACHE PATH "generated_src directory location")
if(NOT EXISTS ${PREGENERATED_SOURCE_DIR}/Python/swigwordsize32/RobotRaconteurPythonPYTHON_wrap.cxx
   OR NOT EXISTS ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize32/RobotRaconteurPythonPYTHON_wrap.cxx
   OR NOT EXISTS ${PREGENERATED_SOURCE_DIR}/Java/swigwordsize32/com/robotraconteur/RobotRaconteurJavaJAVA_wrap.cxx
   OR NOT EXISTS ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize32/RobotRaconteurNETCSHARP_wrap.cxx)
   message(FATAL_ERROR "Invalid PREGENERATED_SOURCE_DIR")
endif()
else()
include(${CMAKE_SOURCE_DIR}/cmake/package_source_swig.cmake)
endif()
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "cmake/Config.cmake.in"
  "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
  PATH_VARS
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_BINDIR  
)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake" 
    VERSION ${ROBOTRACONTEUR_VERSION} COMPATIBILITY AnyNewerVersion)

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.git/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.vs/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.vscode/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.github/")
#list(APPEND CPACK_SOURCE_IGNORE_FILES "/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "^${CMAKE_SOURCE_DIR}/build[\\\\-_/]")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.cache")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.gitignore")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.travis.yml")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.clang-format")

set(CPACK_PACKAGE_VERSION ${ROBOTRACONTEUR_VERSION_SEMVER})

set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_SOURCE_DIR};/;${CMAKE_BINARY_DIR}/generated_src;/generated_src" )
set(CPACK_SOURCE_TBZ2 OFF CACHE BOOL "" FORCE)
set(CPACK_SOURCE_TXZ OFF CACHE BOOL "" FORCE)
set(CPACK_SOURCE_TZ OFF CACHE BOOL "" FORCE)
if(NOT WIN32)
set(CPACK_SOURCE_TGZ ON CACHE BOOL "" FORCE)
endif()
set(CPACK_SOURCE_ZIP ON CACHE BOOL "" FORCE)

include(CPack)

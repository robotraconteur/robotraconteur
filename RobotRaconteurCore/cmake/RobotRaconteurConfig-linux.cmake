
include(FindPackageHandleStandardArgs)

SET (Boost_ADDITIONAL_VERSIONS @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@.0 @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@)
SET (Boost_USE_MULTITHREADED ON)
SET (Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS date_time filesystem system regex chrono atomic thread random REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenSSL)	
include_directories(${OPENSSL_INCLUDE_DIR})

string(REGEX MATCH "^[0-9]+\\.[0-9]+" OPENSSL_VERSION_MAJOR_MINOR ${OPENSSL_VERSION})

if (NOT "${OPENSSL_FOUND}" OR NOT "${OPENSSL_VERSION_MAJOR_MINOR}" STREQUAL "@OPENSSL_VERSION_MAJOR@.@OPENSSL_VERSION_MINOR@")
message(FATAL_ERROR "Invalid OpenSSL Version expect @OPENSSL_VERSION_MAJOR@.@OPENSSL_VERSION_MINOR@" )
endif()

if (NOT ${Boost_MAJOR_VERSION} STREQUAL @Boost_MAJOR_VERSION@ OR NOT ${Boost_MINOR_VERSION} STREQUAL @Boost_MINOR_VERSION@)
message(FATAL_ERROR "Invalid Boost Version expected @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@")
endif()


get_filename_component(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
if (NOT RobotRaconteur_INCLUDE_DIRS)
set(RobotRaconteur_INCLUDE_DIRS  "${CMAKE_CURRENT_LIST_DIR}/include" CACHE PATH "" FORCE)
endif()
mark_as_advanced(RobotRaconteur_INCLUDE_DIRS)
include_directories(${RobotRaconteur_INCLUDE_DIRS})

if (NOT RobotRaconteur_LIBRARY)
set(RobotRaconteur_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/libRobotRaconteurCore.a" CACHE FILEPATH "" FORCE)
endif()
mark_as_advanced(RobotRaconteur_LIBRARY)

if (NOT EXISTS "${RobotRaconteur_INCLUDE_DIRS}/RobotRaconteur.h")
set(RobotRaconteur_INCLUDE_DIRS "RobotRaconteur_INCLUDE_DIRS-NOTFOUND" CACHE PATH "" FORCE)
endif()

if (NOT EXISTS "${RobotRaconteur_LIBRARY}")
set(RobotRaconteur_LIBRARY "RobotRaconteur_LIBRARY-NOTFOUND" CACHE FILEPATH "" FORCE)
endif()

set (RobotRaconteur_EXTRA_LIBRARIES ${RobotRaconteur_EXTRA_LIBRARIES} ${OPENSSL_LIBRARIES} pthread rt z )
set (RobotRaconteur_MINIMAL_EXTRA_LIBRARIES ${RobotRaconteur_EXTRA_LIBRARIES})
set (RobotRaconteur_EXTRA_LIBRARIES ${Boost_LIBRARIES} ${RobotRaconteur_EXTRA_LIBRARIES})

set(RobotRaconteur_GEN ${CMAKE_CURRENT_LIST_DIR}/bin/RobotRaconteurGen)

find_package_handle_standard_args(RobotRaconteur DEFAULT_MSG RobotRaconteur_INCLUDE_DIRS RobotRaconteur_LIBRARY Boost_DATE_TIME_FOUND Boost_FILESYSTEM_FOUND Boost_SYSTEM_FOUND Boost_REGEX_FOUND Boost_CHRONO_FOUND Boost_ATOMIC_FOUND Boost_THREAD_FOUND)

if (NOT RobotRaconteur_FIND_QUIETLY)
message(STATUS "Robot Raconteur version: ${RobotRaconteur_VERSION}")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurGenerateThunk.cmake)

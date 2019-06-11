
include(FindPackageHandleStandardArgs)

SET (Boost_ADDITIONAL_VERSIONS @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@.0 @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@)
SET (Boost_USE_MULTITHREADED ON)
SET (Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS date_time filesystem system regex chrono atomic thread random REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

string(REGEX MATCH "^[0-9]+\\.[0-9]+" OPENSSL_VERSION_MAJOR_MINOR ${OPENSSL_VERSION})

if (NOT "${OPENSSL_FOUND}" OR NOT "${OPENSSL_VERSION_MAJOR_MINOR}" STREQUAL "@OPENSSL_VERSION_MAJOR@.@OPENSSL_VERSION_MINOR@")
message(FATAL_ERROR "Invalid OpenSSL Version expect @OPENSSL_VERSION_MAJOR@.@OPENSSL_VERSION_MINOR@" )
endif()

if (NOT ${Boost_MAJOR_VERSION} STREQUAL @Boost_MAJOR_VERSION@ OR NOT ${Boost_MINOR_VERSION} STREQUAL @Boost_MINOR_VERSION@)
message(FATAL_ERROR "Invalid Boost Version expected @Boost_MAJOR_VERSION@.@Boost_MINOR_VERSION@")
endif()

if ("${OPENSSL_CRYPTO_LIBRARY}" STREQUAL "/usr/lib/libcrypto.dylib")
message(FATAL_ERROR "/usr/lib/libcrypto.dylib from OSX is not valid. Please select the appropiate 1.0.1 OpenSSL library")
endif()

if ("${OPENSSL_SSL_LIBRARY}" STREQUAL "/usr/lib/libssl.dylib")
message(FATAL_ERROR "/usr/lib/libssl.dylib from OSX is not valid. Please select the appropiate 1.0.1 OpenSSL library")
endif()

include(CMakeFindFrameworks)
CMAKE_FIND_FRAMEWORKS(CoreFoundation)
CMAKE_FIND_FRAMEWORKS(Security)
include_directories(${CoreFoundation_FRAMEWORKS}/Headers ${Security_FRAMEWORKS}/Headers)

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

set (RobotRaconteur_EXTRA_LIBRARIES ${RobotRaconteur_EXTRA_LIBRARIES} ${CoreFoundation_FRAMEWORKS}/CoreFoundation ${Security_FRAMEWORKS}/Security ${OPENSSL_LIBRARIES} c++ )
set (RobotRaconteur_MINIMAL_EXTRA_LIBRARIES ${RobotRaconteur_EXTRA_LIBRARIES})
set (RobotRaconteur_EXTRA_LIBRARIES ${Boost_LIBRARIES} ${RobotRaconteur_EXTRA_LIBRARIES})

set(RobotRaconteur_GEN ${CMAKE_CURRENT_LIST_DIR}/bin/RobotRaconteurGen)

find_package_handle_standard_args(RobotRaconteur DEFAULT_MSG RobotRaconteur_INCLUDE_DIRS RobotRaconteur_LIBRARY CoreFoundation_FRAMEWORKS Security_FRAMEWORKS Boost_DATE_TIME_FOUND Boost_FILESYSTEM_FOUND Boost_SYSTEM_FOUND Boost_REGEX_FOUND Boost_CHRONO_FOUND Boost_ATOMIC_FOUND Boost_THREAD_FOUND)

if (NOT RobotRaconteur_FIND_QUIETLY)
message(STATUS "Robot Raconteur version: ${RobotRaconteur_VERSION}")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurGenerateThunk.cmake)

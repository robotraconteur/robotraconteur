cmake_minimum_required(VERSION 3.3)
project(RobotRaconteur)

include(cmake/CMakeMacros.cmake)
include(CTest)

include(CheckTypeSize)
check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)

CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h" "${CMAKE_BINARY_DIR}/RobotRaconteurConfig.h" COPYONLY)

FILE (STRINGS "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h" RobotRaconteurConfigVersion REGEX "#define +ROBOTRACONTEUR_VERSION +\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
IF ("${RobotRaconteurConfigVersion}" STREQUAL "" )
MESSAGE(FATAL_ERROR "Could not read Robot Raconteur version.")
ENDIF()
STRING (REGEX REPLACE "#define ROBOTRACONTEUR_VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" "\\1" RobotRaconteurVersion ${RobotRaconteurConfigVersion})
MESSAGE(STATUS "Robot Raconteur version: ${RobotRaconteurVersion}")

SET(RobotRaconteur_USE_SHARED_CORE_LIB FALSE CACHE BOOL "Build using shared Robot Raconteur core library")

SET(RobotRaconteur_DISABLE_MESSAGE3 TRUE CACHE BOOL "Disable Robot Raconteur message version 3 by default" )
SET(RobotRaconteur_DISABLE_STRINGTABLE TRUE CACHE BOOL "Disable Robot Raconteur string table by default" )
mark_as_advanced(RobotRaconteur_DISABLE_MESSAGE3 RobotRaconteur_DISABLE_STRINGTABLE)

IF(RobotRaconteur_DISABLE_MESSAGE3)
add_definitions(-DROBOTRACONTEUR_DISABLE_MESSAGE3)
ENDIF()
IF(RobotRaconteur_DISABLE_STRINGTABLE)
add_definitions(-DROBOTRACONTEUR_DISABLE_STRINGTABLE)
ENDIF()

#Find boost and get it ready for use
INCLUDE(FindBoost)
#If(NOT DEFINED Boost_USE_STATIC_LIBS)
#IF(RobotRaconteur_USE_SHARED_CORE_LIB)
#SET (Boost_USE_STATIC_LIBS OFF)
#ELSE()
#SET (Boost_USE_STATIC_LIBS ON)
#ENDIF()
#ENDIF()
SET (Boost_USE_MULTITHREADED ON)
SET (Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost COMPONENTS date_time filesystem system regex chrono atomic thread random program_options REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" ${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_ALL_NO_LIB)

if(WIN32)
	add_definitions( -D_WIN32_WINNT=0x0600 )
	set (ROBOTRACONTEUR_EXTRA_LIBS ${ROBOTRACONTEUR_EXTRA_LIBS} Mswsock.lib ws2_32.lib IPHLPAPI.lib Crypt32.lib Secur32.lib Bcrypt.lib)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)

	
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(OPENSSL_USE_STATIC_LIBS TRUE)
		find_package(OpenSSL REQUIRED)
		include_directories(${OPENSSL_INCLUDE_DIR})
		set (ROBOTRACONTEUR_EXTRA_LIBS ${ROBOTRACONTEUR_EXTRA_LIBS} ${OPENSSL_LIBRARIES} pthread z )

	elseif(${__ANDROID__})
		message(STATUS android)		
		list(REMOVE_ITEM Boost_LIBRARIES ${BOOST_THREAD_LIBRARY}) 		
		set(OPENSSL_USE_STATIC_LIBS TRUE)
		find_package(OpenSSL REQUIRED)
		include_directories(${OPENSSL_INCLUDE_DIR} ${LIBUSB_INCLUDE_DIR})
		set (ROBOTRACONTEUR_EXTRA_LIBS ${ROBOTRACONTEUR_EXTRA_LIBS} ${OPENSSL_LIBRARIES} )
	else()
		set(OPENSSL_USE_STATIC_LIBS FALSE)		
		find_path(DBUS_INCLUDE_DIR dbus.h NAMES dbus/dbus.h PATH_SUFFIXES dbus-1.0)
		find_library(DBUS_LIBRARY dbus-1)
		get_filename_component(DBUS_LIBRARY_DIR ${DBUS_LIBRARY} DIRECTORY )
		find_path(DBUS_INCLUDE_ARCH_DIR dbus/dbus-arch-deps.h HINTS ${DBUS_LIBRARY_DIR}/dbus-1.0/include )		
		find_path(LIBUSB_INCLUDE_DIR libusb.h PATH_SUFFIXES libusb-1.0)		
		find_package(OpenSSL REQUIRED)		
		include_directories(${OPENSSL_INCLUDE_DIR} ${DBUS_INCLUDE_DIR} ${DBUS_INCLUDE_ARCH_DIR} ${LIBUSB_INCLUDE_DIR} )
		set (ROBOTRACONTEUR_EXTRA_LIBS ${ROBOTRACONTEUR_EXTRA_LIBS} ${OPENSSL_LIBRARIES} pthread rt z dl)
	endif()

	if(WIN32)
	 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads")
	endif()
	
endif()

#For mac osx use:
# ./b2 toolset=clang cxxflags="-I -std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" link=static 
#to compile boost

if(APPLE)
	include(CMakeFindFrameworks)
	
	CMAKE_FIND_FRAMEWORKS(CoreFoundation)
	CMAKE_FIND_FRAMEWORKS(CoreServices)
	CMAKE_FIND_FRAMEWORKS(Security)
	include_directories(${CoreFoundation_FRAMEWORKS}/Headers ${Security_FRAMEWORKS}/Headers ${CoreServices_FRAMEWORKS}/headers)
	set(OPENSSL_USE_STATIC_LIBS FALSE)
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	set (ROBOTRACONTEUR_EXTRA_LIBS ${ROBOTRACONTEUR_EXTRA_LIBS} ${CoreFoundation_FRAMEWORKS}/CoreFoundation ${Security_FRAMEWORKS}/Security ${CoreServices_FRAMEWORKS}/CoreServices ${OPENSSL_LIBRARIES} c++)
endif()

include(GNUInstallDirs)

if (NOT DEFINED RobotRaconteur_DIR)
add_subdirectory(RobotRaconteurCore)
set(RobotRaconteur_CORE_LIBRARY RobotRaconteurCore)
else()
find_package(RobotRaconteur)
set(RobotRaconteur_CORE_LIBRARY ${RobotRaconteur_LIBRARY})
endif()

set (ROBOTRACONTEUR_ROS "" CACHE BOOL "Build Robot Raconteur for ROS workspace")
if(ROBOTRACONTEUR_ROS)
add_definitions(-DROBOTRACONTEUR_ROS)
set(BUILD_GEN true CACHE BOOL "Build RobotRaconteurGen")
set(BUILD_TEST true CACHE BOOL "Build RobotRaconteurTest")
set(BUILD_PYTHON true CACHE BOOL "Build Python wrappers")
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
write_basic_package_version_file("${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Version.cmake" 
    VERSION ${RobotRaconteurVersion} COMPATIBILITY AnyNewerVersion)

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.git/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.vs/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/\.cache")

string(REPLACE "." ";" RobotRaconteurVersionList ${RobotRaconteurVersion})
list(GET RobotRaconteurVersionList 0 CPACK_PACKAGE_VERSION_MAJOR)
list(GET RobotRaconteurVersionList 1 CPACK_PACKAGE_VERSION_MINOR)
list(GET RobotRaconteurVersionList 2 CPACK_PACKAGE_VERSION_PATCH)

set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_SOURCE_DIR};/;${CMAKE_BINARY_DIR}/generated_src;/generated_src" )

include(CPack)

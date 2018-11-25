
if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fPIC" )
endif()

IF (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275")
ENDIF()

set(RobotRaconteurCore_src
	src/AutoResetEvent.cpp 
	src/CallbackMember.cpp 
	src/DataTypes.cpp 
	src/Endpoint.cpp 
	src/Error.cpp
	src/IOUtils.cpp
	src/ThreadPool.cpp
	src/MemoryMember.cpp
	src/Message.cpp
	src/NodeID.cpp
	src/PipeMember.cpp
	src/PipeMember_private.h
	src/RobotRaconteurNode.cpp
	src/RobotRaconteurNode_connector.cpp
	src/RobotRaconteurNode_connector_private.h
	src/RobotRaconteurServiceIndex_stubskel.cpp
	src/Client.cpp
	src/Service.cpp
	src/Service_lock.cpp
	src/Service_lock_private.h
	src/ServiceStructure.cpp
	src/Security.cpp
	src/ServiceDefinition.cpp
	src/ServiceFactory.cpp
	src/ServiceIndexer.cpp
	src/TcpTransport_private.h
	src/TcpTransport.cpp
	src/Transport.cpp
	src/WireMember.cpp
	src/WireMember_private.h
	src/ASIOStreamBaseTransport.h
	src/ASIOStreamBaseTransport.cpp
	src/Timer.cpp
	src/AsyncUtils.cpp
	src/LocalTransport.cpp
	src/LocalTransport_private.h	
	src/HardwareTransport.cpp
	src/HardwareTransport_private.h
	src/websocket_stream.hpp
	src/StringTable.cpp
	src/AsyncMessageReader.cpp
	src/AsyncMessageReader.h
	src/AsyncMessageWriter.cpp
	src/AsyncMessageWriter.h
	src/AsyncMessageIO.cpp
	src/Discovery.cpp
	src/Discovery_private.h
	src/Subscription.cpp
	src/Subscription_private.h
	src/Generator.cpp
)

set(RobotRaconteurCore_header
	include/RobotRaconteur/RobotRaconteurConfig.h 
	include/RobotRaconteur/AutoResetEvent.h 
	include/RobotRaconteur/CallbackMember.h 
	include/RobotRaconteur/DataTypes.h 
	include/RobotRaconteur/Endpoint.h 
	include/RobotRaconteur/Error.h
	include/RobotRaconteur/ErrorUtil.h
	include/RobotRaconteur/IOUtils.h
	include/RobotRaconteur/MemoryMember.h
	include/RobotRaconteur/Message.h
	include/RobotRaconteur/NodeID.h
	include/RobotRaconteur/PipeMember.h
	include/RobotRaconteur/RobotRaconteurNode.h
	include/RobotRaconteur/RobotRaconteurServiceIndex.h
	include/RobotRaconteur/RobotRaconteurServiceIndex_stubskel.h
	include/RobotRaconteur/Client.h
	include/RobotRaconteur/Service.h
	include/RobotRaconteur/ServiceStructure.h
	include/RobotRaconteur/Security.h
	include/RobotRaconteur/ServiceDefinition.h
	include/RobotRaconteur/ServiceFactory.h
	include/RobotRaconteur/ServiceIndexer.h
	include/RobotRaconteur/TcpTransport.h
	include/RobotRaconteur/Transport.h
	include/RobotRaconteur/WireMember.h
	include/RobotRaconteur.h
	include/RobotRaconteur/RobotRaconteurConstants.h
	include/RobotRaconteur/AsyncUtils.h
	include/RobotRaconteur/ThreadPool.h
	include/RobotRaconteur/Timer.h
	include/RobotRaconteur/LocalTransport.h	
	include/RobotRaconteur/HardwareTransport.h
	include/RobotRaconteur/StringTable.h
	include/RobotRaconteur/AsyncMessageIO.h
	include/RobotRaconteur/Discovery.h 
	include/RobotRaconteur/Subscription.h
	include/RobotRaconteur/Generator.h
)

if (WIN32)
set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/LocalTransport_win.cpp src/LocalTransport_win_private.h src/HardwareTransport_win.cpp src/HardwareTransport_win_private.h src/HardwareTransport_usbcommon.cpp src/HardwareTransport_usbcommon_private.h src/HardwareTransport_bluetoothcommon_private.h src/HardwareTransport_discoverycommon_private.h )
endif()

if ("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT(${__ANDROID__}))
set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/HardwareTransport_linux.cpp src/HardwareTransport_linux_private.h src/HardwareTransport_usbcommon.cpp src/HardwareTransport_usbcommon_private.h src/HardwareTransport_libusb.cpp src/HardwareTransport_libusb_private.h src/LocalTransport_linux.cpp src/LocalTransport_linux_private.h)
endif()

if(CMAKE_GENERATOR STREQUAL Xcode)
set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/LocalTransport_darwin.cpp src/LocalTransport_darwin_private.h)
endif()

if (WIN32 AND NOT(ROBOTRACONTEUR_USE_OPENSSL))

set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/TlsSchannelStreamAdapter.cpp src/TlsSchannelStreamAdapter.h src/HardwareTransport_win.cpp src/HardwareTransport_win_private.h)
add_definitions(-DROBOTRACONTEUR_USE_SCHANNEL)
else()
set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/OpenSSLAuthContext.cpp src/OpenSSLAuthContext.h)
add_definitions(-DROBOTRACONTEUR_USE_OPENSSL)
endif()

IF (${__ANDROID__})
set(RobotRaconteurCore_src ${RobotRaconteurCore_src} ${GETIFADDR_SOURCE_FILE} src/HardwareTransport_usbcommon_private.h src/HardwareTransport_usbcommon.cpp src/HardwareTransport_android_private.h src/HardwareTransport_android.cpp)
include_directories(${LIBUSB_INCLUDE_DIR})
ENDIF()

IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_EXPORTS)
ENDIF()
ENDIF()

IF(MSVC)
add_definitions(-DROBOTRACONTEUR_CORE_USE_STDAFX)
ADD_MSVC_PRECOMPILED_HEADER(stdafx.h src/stdafx.cpp RobotRaconteurCore_src)
ENDIF()

IF (RobotRaconteur_USE_SHARED_CORE_LIB)
STRING(REPLACE "." "." RobotRaconteur_LIB_VERSION ${RobotRaconteurVersion})
SET(RobotRaconteur_BOOST_LIB_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
IF (MSVC)
IF (MSVC_VERSION EQUAL 1600)
SET(RobotRaconteur_MSVC_LIB_VERSION 100)
ENDIF()
IF (MSVC_VERSION EQUAL 1700)
SET(RobotRaconteur_MSVC_LIB_VERSION 110)
ENDIF()
IF (MSVC_VERSION EQUAL 1800)
SET(RobotRaconteur_MSVC_LIB_VERSION 120)
ENDIF()
IF (MSVC_VERSION EQUAL 1900)
SET(RobotRaconteur_MSVC_LIB_VERSION 140)
ENDIF()
SET(RobotRaconteur_SHARED_CORE_LIB_NAME "RobotRaconteurCore-vc${RobotRaconteur_MSVC_LIB_VERSION}-boost${RobotRaconteur_BOOST_LIB_VERSION}-${RobotRaconteur_LIB_VERSION}")
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
STRING(REPLACE "." "" RobotRaconteur_GCC_LIB_VERSION ${CMAKE_CXX_COMPILER_VERSION})
SET(RobotRaconteur_SHARED_CORE_LIB_NAME "RobotRaconteurCore-gcc${RobotRaconteur_GCC_LIB_VERSION}-boost${RobotRaconteur_BOOST_LIB_VERSION}-${RobotRaconteur_LIB_VERSION}")
ENDIF()

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
STRING(REPLACE "." "" RobotRaconteur_CLANG_LIB_VERSION ${CMAKE_CXX_COMPILER_VERSION})
SET(RobotRaconteur_SHARED_CORE_LIB_NAME "RobotRaconteurCore-clang${RobotRaconteur_CLANG_LIB_VERSION}-boost${RobotRaconteur_BOOST_LIB_VERSION}-${RobotRaconteur_LIB_VERSION}")
ENDIF()

MESSAGE(STATUS Robot Raconteur Shared Library Name: ${RobotRaconteur_SHARED_CORE_LIB_NAME})
ENDIF()

link_directories("${Boost_LIBRARY_DIRS}")
include_directories("${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" ${Boost_INCLUDE_DIRS})

IF(RobotRaconteur_USE_SHARED_CORE_LIB)
add_library(RobotRaconteurCore SHARED ${RobotRaconteurCore_src} ${RobotRaconteurCore_header} )
target_link_libraries(RobotRaconteurCore ${Boost_LIBRARIES} ${ROBOTRACONTEUR_EXTRA_LIBS})
SET_TARGET_PROPERTIES(RobotRaconteurCore PROPERTIES OUTPUT_NAME ${RobotRaconteur_SHARED_CORE_LIB_NAME})
SET_TARGET_PROPERTIES(RobotRaconteurCore PROPERTIES OUTPUT_NAME_DEBUG ${RobotRaconteur_SHARED_CORE_LIB_NAME}-d)
ELSE()
add_library(RobotRaconteurCore STATIC ${RobotRaconteurCore_src} ${RobotRaconteurCore_header} )
SET_TARGET_PROPERTIES(RobotRaconteurCore PROPERTIES PREFIX lib)
ENDIF()

file(COPY "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" DESTINATION "${CMAKE_BINARY_DIR}/out/")
if (WIN32)
file(COPY "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" DESTINATION "${CMAKE_BINARY_DIR}/out_debug/")
ENDIF()

RRSetTargetDirs(RobotRaconteurCore "bin" "lib")

SET_TARGET_PROPERTIES(RobotRaconteurCore PROPERTIES LINKER_LANGUAGE CXX)

export(TARGETS RobotRaconteurCore FILE ${CMAKE_BINARY_DIR}/RobotRaconteurCoreTargets.cmake)
install(TARGETS RobotRaconteurCore EXPORT RobotRaconteurCoreTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h*")
install(EXPORT RobotRaconteurCoreTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

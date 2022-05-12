set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Build using shared Robot Raconteur core library")

set(RobotRaconteur_DISABLE_MESSAGE4 FALSE CACHE BOOL "Disable Robot Raconteur message version 3 by default")
set(RobotRaconteur_DISABLE_STRINGTABLE FALSE CACHE BOOL "Disable Robot Raconteur string table by default")
mark_as_advanced(RobotRaconteur_DISABLE_MESSAGE4 RobotRaconteur_DISABLE_STRINGTABLE)

include(FindBoost)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL "")
find_package(
    Boost
    COMPONENTS date_time
               filesystem
               system
               regex
               chrono
               atomic
               thread
               random
               program_options
    REQUIRED)

if(WIN32 AND ROBOTRACONTEUR_USE_OPENSSL)
    find_package(OpenSSL REQUIRED)
endif()

if(ANDROID)
    # TODO: Determine how to handle this list(REMOVE_ITEM Boost_LIBRARIES ${BOOST_THREAD_LIBRARY})
    find_package(OpenSSL REQUIRED)
endif()

if("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)
    find_path(DBUS_INCLUDE_DIR dbus.h NAMES dbus/dbus.h PATH_SUFFIXES dbus-1.0)
    find_library(DBUS_LIBRARY dbus-1)
    get_filename_component(DBUS_LIBRARY_DIR ${DBUS_LIBRARY} DIRECTORY)
    find_path(DBUS_INCLUDE_ARCH_DIR dbus/dbus-arch-deps.h HINTS ${DBUS_LIBRARY_DIR}/dbus-1.0/include)
    find_path(LIBUSB_INCLUDE_DIR libusb.h PATH_SUFFIXES libusb-1.0)
    find_package(OpenSSL REQUIRED)
endif()

if(APPLE)
    find_library(COREFOUNDATION_FRAMEWORK "CoreFoundation")
    if(NOT COREFOUNDATION_FRAMEWORK)
        message(FATAL_ERROR "CoreFoundation framework not found")
    endif()

    find_library(SECURITY_FRAMEWORK "Security")
    if(NOT SECURITY_FRAMEWORK)
        message(FATAL_ERROR "Security framework not found")
    endif()

    find_library(CORESERVICES_FRAMEWORK "CoreServices")
    if(NOT CORESERVICES_FRAMEWORK)
        message(FATAL_ERROR "CoreFoundation framework not found")
    endif()

    find_package(OpenSSL REQUIRED)
endif()

if(UNIX AND NOT "${CMAKE_SYSTEM}" MATCHES "Linux")
    # TODO: Determine how to handle this list(REMOVE_ITEM Boost_LIBRARIES ${BOOST_THREAD_LIBRARY})
    find_package(OpenSSL REQUIRED)
endif()

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
    src/NodeSetup.cpp
    src/DataTypesPacking.cpp
    src/Logging.cpp
    src/IntraTransport.cpp
    src/Tap.cpp
    src/BroadcastDownsampler.cpp)

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
    include/RobotRaconteur/NodeSetup.h
    include/RobotRaconteur/DataTypesPacking.h
    include/RobotRaconteur/ASIOStreamBaseTransport.h
    include/RobotRaconteur/Logging.h
    include/RobotRaconteur/IntraTransport.h
    include/RobotRaconteur/Tap.h
    include/RobotRaconteur/BroadcastDownsampler.h)

if(WIN32)
    set(RobotRaconteurCore_src
        ${RobotRaconteurCore_src}
        src/LocalTransport_win.cpp
        src/LocalTransport_win_private.h
        src/HardwareTransport_win.cpp
        src/HardwareTransport_win_private.h
        src/HardwareTransport_usbcommon.cpp
        src/HardwareTransport_usbcommon_private.h
        src/HardwareTransport_bluetoothcommon_private.h
        src/HardwareTransport_discoverycommon_private.h)
endif()

if("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)
    set(RobotRaconteurCore_src
        ${RobotRaconteurCore_src}
        src/HardwareTransport_linux.cpp
        src/HardwareTransport_linux_private.h
        src/HardwareTransport_usbcommon.cpp
        src/HardwareTransport_usbcommon_private.h
        src/HardwareTransport_libusb.cpp
        src/HardwareTransport_libusb_private.h
        src/LocalTransport_linux.cpp
        src/LocalTransport_linux_private.h)
endif()

if(APPLE)
    set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/LocalTransport_darwin.cpp
                               src/LocalTransport_darwin_private.h)
endif()

if(WIN32 AND NOT (ROBOTRACONTEUR_USE_OPENSSL))
    set(RobotRaconteurCore_src
        ${RobotRaconteurCore_src} src/TlsSchannelStreamAdapter.cpp src/TlsSchannelStreamAdapter.h
        src/HardwareTransport_win.cpp src/HardwareTransport_win_private.h)
else()
    set(RobotRaconteurCore_src ${RobotRaconteurCore_src} src/OpenSSLAuthContext.cpp src/OpenSSLAuthContext.h)
endif()

if(ANDROID)
    set(RobotRaconteurCore_src ${RobotRaconteurCore_src}) # src/HardwareTransport_usbcommon_private.h
                                                          # src/HardwareTransport_usbcommon.cpp
                                                          # src/HardwareTransport_android_private.h
                                                          # src/HardwareTransport_android.cpp)
endif()

if(BUILD_SHARED_LIBS)
    string(REPLACE "." "." RobotRaconteur_LIB_VERSION ${ROBOTRACONTEUR_VERSION})
    set(RobotRaconteur_SHARED_CORE_VERSION "${RobotRaconteur_LIB_VERSION}")
endif()

if(BUILD_SHARED_LIBS)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_library(RobotRaconteurCore SHARED ${RobotRaconteurCore_src} ${RobotRaconteurCore_header})
    set_target_properties(
        RobotRaconteurCore
        PROPERTIES VERSION "${RobotRaconteur_SHARED_CORE_VERSION}" SOVERSION "${RobotRaconteur_SHARED_CORE_VERSION}"
                   COMPILE_DEFINITIONS "ROBOTRACONTEUR_CORE_EXPORTS")
else()
    add_library(RobotRaconteurCore STATIC ${RobotRaconteurCore_src} ${RobotRaconteurCore_header})
    set_target_properties(RobotRaconteurCore PROPERTIES PREFIX lib)
endif()

target_link_libraries(
    RobotRaconteurCore
    PUBLIC Boost::boost
           Boost::disable_autolinking
           Boost::date_time
           Boost::filesystem
           Boost::system
           Boost::regex
           Boost::chrono
           Boost::atomic
           Boost::thread
           Boost::random
           Boost::program_options)

if(RobotRaconteur_DISABLE_MESSAGE4)
    target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_DISABLE_MESSAGE4)
endif()
if(RobotRaconteur_DISABLE_STRINGTABLE)
    target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_DISABLE_STRINGTABLE)
endif()

if(WIN32)
    target_compile_definitions(RobotRaconteurCore PUBLIC _WIN32_WINNT=0x0600 NOMINMAX=1)
    if(ROBOTRACONTEUR_USE_OPENSSL)
        target_link_libraries(
            RobotRaconteurCore
            PRIVATE Mswsock.lib
                    ws2_32.lib
                    IPHLPAPI.lib
                    Bcrypt.lib
                    Crypt32.lib
                    OpenSSL::SSL
                    OpenSSL::Crypto)
        target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
    else()
        target_link_libraries(
            RobotRaconteurCore
            PRIVATE Mswsock.lib
                    ws2_32.lib
                    IPHLPAPI.lib
                    Bcrypt.lib
                    Crypt32.lib
                    Secur32.lib
                    Bcrypt.lib)
        target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_USE_SCHANNEL)
    endif()

    if(CMAKE_COMPILER_IS_GNUCXX)
        target_compile_options(RobotRaconteurCore PRIVATE "-mthreads")
    endif()

    if(BUILD_SHARED_LIBS)
        target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_CORE_EXPORTS
                                   INTERFACE ROBOTRACONTEUR_CORE_IMPORTS)
    endif()
endif()

if(APPLE)
    target_link_libraries(RobotRaconteurCore PRIVATE "${COREFOUNDATION_FRAMEWORK}" "${SECURITY_FRAMEWORK}"
                                                     "${CORESERVICES_FRAMEWORK}" OpenSSL::SSL OpenSSL::Crypto c++)
    target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_include_directories(RobotRaconteurCore PRIVATE ${DBUS_INCLUDE_DIR} ${DBUS_INCLUDE_ARCH_DIR}
                                                          ${LIBUSB_INCLUDE_DIR})
    target_link_libraries(RobotRaconteurCore PRIVATE OpenSSL::SSL OpenSSL::Crypto pthread rt z dl)
    target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(ANDROID)
    target_include_directories(RobotRaconteurCore PRIVATE ${LIBUSB_INCLUDE_DIR})
    target_link_libraries(RobotRaconteurCore PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(RobotRaconteurCore PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(RobotRaconteurCore PRIVATE "-fPIC")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(RobotRaconteurCore PRIVATE "-fPIC")
endif()

if(MSVC)
    target_compile_options(RobotRaconteurCore PRIVATE "/wd4251" "/wd4275" "/bigobj")
endif()

target_include_directories(
    RobotRaconteurCore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
                              $<INSTALL_INTERFACE:include/> # <prefix>/include/
)

if(${CMAKE_VERSION} VERSION_GREATER "3.16.0")
    target_precompile_headers(RobotRaconteurCore PRIVATE ${RobotRaconteurCore_header})
    set_source_files_properties(src/LocalTransport.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE)
    set_source_files_properties(src/Tap.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE)
endif()

file(COPY "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" DESTINATION "${CMAKE_BINARY_DIR}/out/")
if(WIN32)
    file(COPY "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include" DESTINATION "${CMAKE_BINARY_DIR}/out_debug/")
endif()

rrsettargetdirs(RobotRaconteurCore "bin" "lib")

set_target_properties(RobotRaconteurCore PROPERTIES LINKER_LANGUAGE CXX)

export(TARGETS RobotRaconteurCore FILE ${CMAKE_BINARY_DIR}/RobotRaconteurCoreTargets.cmake)
install(TARGETS RobotRaconteurCore EXPORT RobotRaconteurCoreTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h*")
install(EXPORT RobotRaconteurCoreTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

# https://p5r.uk/blog/2014/cmake-doxygen.html
if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation")
    endif()

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/cpp)
    configure_file(${CMAKE_SOURCE_DIR}/docs/cpp/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(
        RobotRaconteurCore_doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()

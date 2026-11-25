rr_test_add_executable(
    robotraconteur_misc_tests
    SRC
    misc_tests.cpp
    DEPS
    robotraconteur_test_lfsr
    RobotRaconteurCore
    robotraconteur_test_cpp_common
    GTest::Main)

# Parses USB descriptors with the private USB transport parser, only compiled into the static core library on these
# platforms
if(NOT BUILD_SHARED_LIBS AND (WIN32 OR ("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)))
    rr_test_add_executable(robotraconteur_usb_descriptor_tests SRC usb_descriptor_tests.cpp DEPS RobotRaconteurCore
                           GTest::Main)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_sha1_leak_test
        SRC
        websocket_sha1_leak_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_sha1_leak_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_sha1_leak_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_ping_test
        SRC
        websocket_ping_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_ping_test PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_ping_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_masked_empty_frame_test
        SRC
        websocket_masked_empty_frame_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_masked_empty_frame_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_masked_empty_frame_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_handshake_trailing_data_test
        SRC
        websocket_handshake_trailing_data_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_handshake_trailing_data_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_handshake_trailing_data_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_client_short_write_test
        SRC
        websocket_client_short_write_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_client_short_write_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_client_short_write_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_server_short_write_test
        SRC
        websocket_server_short_write_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_server_short_write_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_server_short_write_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

if(NOT WIN32)
    find_package(OpenSSL REQUIRED)
    rr_test_add_executable(
        robotraconteur_websocket_header_end_test
        SRC
        websocket_header_end_test.cpp
        DEPS
        RobotRaconteurCore
        OpenSSL::Crypto
        GTest::Main)
    target_include_directories(robotraconteur_websocket_header_end_test
                               PRIVATE ${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src)
    target_compile_definitions(robotraconteur_websocket_header_end_test PRIVATE ROBOTRACONTEUR_USE_OPENSSL)
endif()

rr_test_add_executable(robotraconteur_tcp_accept_timeout_test SRC tcp_accept_timeout_test.cpp DEPS RobotRaconteurCore
                       GTest::Main)

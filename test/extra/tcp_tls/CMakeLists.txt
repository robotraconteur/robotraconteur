robotraconteur_generate_thunk(RR_THUNK_SRCS RR_THUNK_HDRS experimental.tls_test.robdef MASTER_HEADER INCLUDE_DIRS
                              ${ROBOTRACONTEUR_TEST_ROBDEF_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

rr_test_add_executable(
    robotraconteur_tls_test_cpp
    SRC
    tls_test.cpp
    ${RR_THUNK_SRCS}
    ${RR_THUNK_HDRS}
    DEPS
    RobotRaconteurCore
    GTest::Main)

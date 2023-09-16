add_subdirectory(tcp_tls)

if(BUILD_PYTHON3)
    if(WIN32)
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3\\\;${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3:${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
    set(RR_PYTHON_TEST_CMD_ARGS
        "-E env PYTHONPATH=${RR_TEST_PYTHON_PATH} ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/robdef \"${PYTHON3_EXECUTABLE}\" "
    )

    rrconfiguretest(test_python3_extra_tests "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}")

endif()

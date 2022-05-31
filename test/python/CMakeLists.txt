if(BUILD_PYTHON3)
    if(WIN32)
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3\\\;${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3:${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
    set(RR_PYTHON_TEST_CMD_ARGS
        "-E env PYTHONPATH=${RR_TEST_PYTHON_PATH} ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_SOURCE_DIR}/test/robdef \"${PYTHON3_EXECUTABLE}\" "
    )

    rrconfiguretest(
        test_python3_module
        "${RR_PYTHON_TEST_CMD}"
        "test_prog"
        NOCTEST
        NODEBUG
        ARGS
        "${RR_PYTHON_TEST_CMD_ARGS} -m")
    rrconfiguretest(test_python3_pytest "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_SOURCE_DIR}")

endif()

if(BUILD_PYTHON)
    if(WIN32)
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python\\\;${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python:${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
    set(RR_PYTHON_TEST_CMD_ARGS
        "-E env PYTHONPATH=${RR_TEST_PYTHON_PATH} ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_SOURCE_DIR}/test/robdef \"${PYTHON_EXECUTABLE}\" "
    )

    rrconfiguretest(
        test_python_module
        "${RR_PYTHON_TEST_CMD}"
        "test_prog"
        NOCTEST
        NODEBUG
        ARGS
        "${RR_PYTHON_TEST_CMD_ARGS} -m")
    rrconfiguretest(test_python_pytest "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_SOURCE_DIR}")

endif()

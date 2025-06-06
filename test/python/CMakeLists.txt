if(BUILD_PYTHON3)
    if(CMAKE_VERSION VERSION_GREATER 3.14 OR CMAKE_VERSION VERSION_EQUAL 3.14)
        if(PYTHON3_EXECUTABLE)
            set(Python3_EXECUTABLE ${PYTHON3_EXECUTABLE} CACHE FILEPATH "Python 3 executable")
        endif()
        find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
        set(PYTHON3_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
        set(PYTHON3_LIBRARIES ${Python3_LIBRARIES})
        set(PYTHON3_EXECUTABLE ${Python3_EXECUTABLE})
    endif()
    if(WIN32)
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3\\\;${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3:${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
    set(RR_PYTHON_TEST_CMD_ARGS
        "-E env PYTHONPATH=${RR_TEST_PYTHON_PATH} ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/robdef \"${PYTHON3_EXECUTABLE}\" "
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
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}")

endif()

if(BUILD_PYTHON)

    if(CMAKE_VERSION VERSION_GREATER 3.14 OR CMAKE_VERSION VERSION_EQUAL 3.14)
        if(PYTHON_EXECUTABLE)
            set(Python2_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE FILEPATH "Python executable")
        endif()
        find_package(Python2 COMPONENTS Interpreter Development NumPy REQUIRED)
        set(PYTHON_INCLUDE_DIRS ${Python2_INCLUDE_DIRS})
        set(PYTHON_LIBRARIES ${Python2_LIBRARIES})
        set(PYTHON_EXECUTABLE ${Python2_EXECUTABLE})
    endif()

    if(WIN32)
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python\\\;${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(RR_TEST_PYTHON_PATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python:${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
    set(RR_PYTHON_TEST_CMD_ARGS
        "-E env PYTHONPATH=${RR_TEST_PYTHON_PATH} ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/robdef \"${PYTHON_EXECUTABLE}\" "
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
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}")

endif()

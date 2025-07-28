add_subdirectory(tcp_tls)

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

    rrconfiguretest(test_python3_extra_tests "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS
                    "${RR_PYTHON_TEST_CMD_ARGS} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}")

endif()

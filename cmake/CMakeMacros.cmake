function(RRSetTargetDirs target bindir libdir)

    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${libdir}")
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/out_debug/${libdir}")
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/${libdir}")
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO
                                               "${CMAKE_BINARY_DIR}/out_reldebug/${libdir}")

    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${bindir}")
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/out_debug/${bindir}")
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/${bindir}")
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
                                               "${CMAKE_BINARY_DIR}/out_reldebug/${bindir}")

    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${libdir}")
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/out_debug/${libdir}")
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/${libdir}")
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO
                                               "${CMAKE_BINARY_DIR}/out_reldebug/${libdir}")

endfunction()

function(RRConfigureTest test_name cmd template_in)
    cmake_parse_arguments(RRConfigureTest "NOCTEST;NODEBUG" "FILELIST;ARGS" "" ${ARGN})

    set(OUT_DIR_NAME_LIST "out;out_reldebug;out_debug")
    foreach(OUT_DIR_NAME ${OUT_DIR_NAME_LIST})
        string(CONFIGURE "${cmd}" abs_cmd1 @ONLY)
        file(TO_NATIVE_PATH "${abs_cmd1}" ROBOTRACONTEUR_TEST_CMD)
        string(CONFIGURE "${RRConfigureTest_ARGS}" ROBOTRACONTEUR_TEST_ARGS @ONLY)
        if(RRConfigureTest_FILELIST)
            string(CONFIGURE "${${RRConfigureTest_FILELIST}}" ROBOTRACONTEUR_TEST_GEN_FILE_LIST @ONLY)
            # file(TO_NATIVE_PATH "${filelist1}" ROBOTRACONTEUR_TEST_GEN_FILE_LIST)
        endif()
        set(ROBOTRACONTEUR_CORE_BIN_DIR ${CMAKE_BINARY_DIR}/${OUT_DIR_NAME}/bin)
        set(ROBOTRACONTEUR_CORE_LIB_DIR ${CMAKE_BINARY_DIR}/${OUT_DIR_NAME}/lib)
        if(WIN32)
            configure_file("${CMAKE_SOURCE_DIR}/test/templates/${template_in}.bat.in"
                           "${CMAKE_BINARY_DIR}/test/${OUT_DIR_NAME}/scripts/${test_name}.bat" @ONLY)
        else()
            configure_file("${CMAKE_SOURCE_DIR}/test/templates/${template_in}.in"
                           "${CMAKE_BINARY_DIR}/test/${OUT_DIR_NAME}/scripts/${test_name}" @ONLY)
        endif()
    endforeach()
    if(WIN32)
        if(NOT RRConfigureTest_NOCTEST)
            if(NOT RRConfigureTest_NODEBUG)
                add_test(
                    NAME ${test_name}
                    COMMAND "${CMAKE_BINARY_DIR}/test/out_debug/scripts/${test_name}.bat"
                    CONFIGURATIONS Debug
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out_debug/scripts")
            endif()
            add_test(
                NAME "${test_name}_rel"
                COMMAND "${CMAKE_BINARY_DIR}/test/out/scripts/${test_name}.bat"
                CONFIGURATIONS Release
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out/scripts")
            add_test(
                NAME "${test_name}_reldeb"
                COMMAND "${CMAKE_BINARY_DIR}/test/out_reldebug/scripts/${test_name}.bat"
                CONFIGURATIONS RelWithDebInfo
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out_reldebug/scripts")
        endif()
    else()
        if(NOT RRConfigureTest_NOCTEST)
            if(NOT RRConfigureTest_NODEBUG)
                add_test(
                    NAME ${test_name}
                    COMMAND "${CMAKE_BINARY_DIR}/test/out_debug/scripts/${test_name}"
                    CONFIGURATIONS Debug
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out_debug/scripts")
            endif()
            add_test(
                NAME "${test_name}_rel"
                COMMAND "${CMAKE_BINARY_DIR}/test/out/scripts/${test_name}"
                CONFIGURATIONS Release
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out/scripts")
            add_test(
                NAME "${test_name}_reldeb"
                COMMAND "${CMAKE_BINARY_DIR}/test/out_reldebug/scripts/${test_name}"
                CONFIGURATIONS RelWithDebInfo
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/out_reldebug/scripts")
        endif()
    endif()
endfunction()

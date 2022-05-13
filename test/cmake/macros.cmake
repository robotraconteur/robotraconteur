function(RR_SET_TEST_TARGET_DIRS target bindir libdir)

set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/out/${libdir}")
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test/out_debug/${libdir}")
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test/out/${libdir}")
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/test/out_reldebug/${libdir}")

set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/out/${bindir}")
set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test/out_debug/${bindir}")
set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test/out/${bindir}")
set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/test/out_reldebug/${bindir}")

set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/out/${libdir}")
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test/out_debug/${libdir}")
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test/out/${libdir}")
set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/test/out_reldebug/${libdir}")

endfunction()


function(RR_TEST_ADD_EXECUTABLE TARGET_NAME)

    cmake_parse_arguments(RR_ARG "" "" "SRC;INCLUDE;DEPS;TEST_EXTRA_ARGS" ${ARGN})
    message("TARGET_NAME ${TARGET_NAME}")
    message("ARGN ${ARGN}")
    message("SRC ${RR_ARG_SRC}")

    add_executable(${TARGET_NAME} ${RR_ARG_SRC})
    if (RR_ARG_INCLUDE)
        target_include_directories(${TARGET_NAME} PUBLIC ${RR_ARG_INCLUDE})
    endif()
    if(RR_ARG_DEPS)
        target_link_libraries(${TARGET_NAME} ${RR_ARG_DEPS})
    endif()
    RR_SET_TEST_TARGET_DIRS(${TARGET_NAME} bin lib)
    gtest_discover_tests(${TARGET_NAME} EXTRA_ARGS ${RR_ARG_TEST_EXTRA_ARGS})
endfunction()

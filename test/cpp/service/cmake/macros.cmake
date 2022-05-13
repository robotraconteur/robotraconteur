function(RR_SERVICE_TEST_ADD_TEST TARGET_NAME)

cmake_parse_arguments(RR_ARG "" "" "SRC" ${ARGN})
RR_TEST_ADD_EXECUTABLE(robotraconteur_test_${TARGET_NAME}
    SRC ${RR_ARG_SRC}
    DEPS robotraconteur_test_service_test_lib    
)
target_include_directories(robotraconteur_test_${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
if (MSVC)
	target_compile_options(robotraconteur_test_${TARGET_NAME} PRIVATE "/bigobj")
endif()
endfunction()

function(RR_SERVICE_TEST_ADD_EXE TARGET_NAME)
cmake_parse_arguments(RR_ARG "" "" "SRC" ${ARGN})
add_executable(robotraconteur_test_${TARGET_NAME}
    ${RR_ARG_SRC})
target_link_libraries(robotraconteur_test_${TARGET_NAME} robotraconteur_test_service_test_lib)
target_include_directories(robotraconteur_test_${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
if (MSVC)
	target_compile_options(robotraconteur_test_${TARGET_NAME} PRIVATE "/bigobj")
endif()
RR_SET_TEST_TARGET_DIRS(robotraconteur_test_${TARGET_NAME} bin lib)
endfunction()
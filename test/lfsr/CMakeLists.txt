add_library(robotraconteur_test_lfsr SHARED src/robotraconteur_test_lfsr.c)
target_include_directories(robotraconteur_test_lfsr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(robotraconteur_test_lfsr_static STATIC src/robotraconteur_test_lfsr.c)
target_include_directories(robotraconteur_test_lfsr_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
    target_compile_definitions(robotraconteur_test_lfsr PRIVATE ROBOTRACONTEUR_TEST_LFSR_EXPORTS
                               INTERFACE ROBOTRACONTEUR_TEST_LFSR_IMPORTS)
endif()

rr_set_test_target_dirs(robotraconteur_test_lfsr bin lib)

add_executable(robotraconteur_test_lfsr_print src/robotraconteur_test_lfsr_print.cpp)
target_link_libraries(robotraconteur_test_lfsr_print robotraconteur_test_lfsr RobotRaconteurCore)

rr_set_test_target_dirs(robotraconteur_test_lfsr_print bin lib)

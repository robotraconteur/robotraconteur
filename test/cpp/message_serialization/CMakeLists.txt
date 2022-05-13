# add_executable(robotraconteur_message_serialization_test src/message_test_util.cpp src/message_serialization_test.cpp)
# target_include_directories(robotraconteur_message_serialization_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(robotraconteur_message_serialization_test robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main)
# RR_SET_TEST_TARGET_DIRS(robotraconteur_message_serialization_test bin lib)
# gtest_discover_tests(robotraconteur_message_serialization_test)

RR_TEST_ADD_EXECUTABLE(robotraconteur_message_serialization_test 
    SRC src/message_test_util.cpp src/message_serialization_test.cpp 
    INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include
    DEPS robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main    
)

# add_executable(robotraconteur_async_message_serialization_test src/message_test_util.cpp src/async_message_serialization_test.cpp)
# target_include_directories(robotraconteur_async_message_serialization_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(robotraconteur_async_message_serialization_test robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main)
# RR_SET_TEST_TARGET_DIRS(robotraconteur_async_message_serialization_test bin lib)
# gtest_discover_tests(robotraconteur_async_message_serialization_test)

RR_TEST_ADD_EXECUTABLE(robotraconteur_async_message_serialization_test
    SRC src/message_test_util.cpp src/async_message_serialization_test.cpp
    INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include
    DEPS robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main
)
RR_TEST_ADD_EXECUTABLE(robotraconteur_misc_tests
    SRC misc_tests.cpp 
    DEPS robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main   
)
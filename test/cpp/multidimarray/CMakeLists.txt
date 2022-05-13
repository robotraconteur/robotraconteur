RR_TEST_ADD_EXECUTABLE(robotraconteur_multidimarray_test 
    SRC multidimarray_test.cpp 
    DEPS robotraconteur_test_lfsr RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main   
)
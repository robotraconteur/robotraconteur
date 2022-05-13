RR_TEST_ADD_EXECUTABLE(robotraconteur_robdef_test 
    SRC robdef_test.cpp 
    DEPS RobotRaconteurCore robotraconteur_test_cpp_common GTest::Main    
)

target_compile_definitions(robotraconteur_robdef_test PUBLIC 
    ROBOTRACONTEUR_TEST_ROBDEF_DIR="${ROBOTRACONTEUR_TEST_ROBDEF_DIR}")
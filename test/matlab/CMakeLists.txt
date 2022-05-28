find_package(Matlab COMPONENTS MAIN_PROGRAM)

set(RR_MATLAB_TEST_CMD "\"${CMAKE_COMMAND}\"")
if(WIN32)
    set(RR_TEST_SERVER_LIB "${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/bin/robotraconteur_test_server_lib.dll")
else()
    set(RR_TEST_SERVER_LIB "${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/lib/robotraconteur_test_server_lib.so")
endif()
set(RR_MATLABPATH "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Matlab")
set(RR_MATLAB_TEST_CMD_ARGS
    "-E env ROBOTRACONTEUR_TEST_SERVER_LIB=\"${RR_TEST_SERVER_LIB}\" ROBOTRACONTEUR_TEST_SERVER_LIB_HFILE=\"${CMAKE_SOURCE_DIR}/test/cpp/service/test_server_lib.h\" ROBOTRACONTEUR_ROBDEF_PATH=\"${CMAKE_SOURCE_DIR}/test/robdef\" MATLABPATH=\"${RR_MATLABPATH}\" \"${Matlab_MAIN_PROGRAM}\" -batch \"cd(fullfile('${CMAKE_CURRENT_SOURCE_DIR}'))\\\; RobotRaconteurMatlabTestLoopback\" "
)

rrconfiguretest(test_matlab_loopback "${RR_MATLAB_TEST_CMD}" "test_prog" ARGS "${RR_MATLAB_TEST_CMD_ARGS}")

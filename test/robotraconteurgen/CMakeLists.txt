add_test(
    NAME RobotRaconteurGen.DocStringCommentTerminator
    COMMAND
        ${CMAKE_COMMAND} -DROBOTRACONTEURGEN=$<TARGET_FILE:RobotRaconteurGen>
        -DROBDEF=${CMAKE_CURRENT_SOURCE_DIR}/experimental.gen_test_docstring.robdef
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/docstring -P ${CMAKE_CURRENT_SOURCE_DIR}/check_docstring.cmake)

add_test(
    NAME RobotRaconteurGen.CppStringEscape
    COMMAND
        ${CMAKE_COMMAND} -DROBOTRACONTEURGEN=$<TARGET_FILE:RobotRaconteurGen>
        -DROBDEF=${CMAKE_CURRENT_SOURCE_DIR}/experimental.gen_test_string_escape.robdef
        -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/string_escape -P ${CMAKE_CURRENT_SOURCE_DIR}/check_string_escape.cmake)

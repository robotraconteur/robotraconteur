# Runs RobotRaconteurGen --lang=cpp on a robdef with string constants that need escaping and checks the generated C++
# string literals. Usage: cmake -DROBOTRACONTEURGEN=<exe> -DROBDEF=<file> -DOUT_DIR=<dir> -P check_string_escape.cmake

file(REMOVE_RECURSE "${OUT_DIR}")
file(MAKE_DIRECTORY "${OUT_DIR}")
execute_process(COMMAND "${ROBOTRACONTEURGEN}" --thunksource --lang=cpp "--output-dir=${OUT_DIR}" "${ROBDEF}"
                RESULT_VARIABLE res)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "RobotRaconteurGen failed: ${res}")
endif()

file(READ "${OUT_DIR}/experimental__gen_test_string_escape.h" header)
# "\x01abc" would be read as the single escape \x01abc; the byte must be followed by the literal "abc"
if(NOT header MATCHES "escape_hex=\"\\\\001abc\";")
    message(FATAL_ERROR "Control character escape is not terminated:\n${header}")
endif()
# "??/" is a trigraph for a backslash in pre-C++17 modes
if(NOT header MATCHES "escape_trigraph=\"what\\\\\\?\\\\\\?/\";")
    message(FATAL_ERROR "Trigraph sequence in string constant is not escaped:\n${header}")
endif()

file(READ "${OUT_DIR}/experimental__gen_test_string_escape_stubskel.cpp" stubskel)
if(stubskel MATCHES "\\?\\?")
    message(FATAL_ERROR "Trigraph sequence in service definition string is not escaped")
endif()

# Runs RobotRaconteurGen --lang=csharp on a robdef with numeric constants and checks that single constants get the F
# suffix (a literal without suffix is a double, which C# does not convert implicitly to float). The Java output of the
# same robdef is compiled and checked by the Java test. Usage: cmake -DROBOTRACONTEURGEN=<exe> -DROBDEF=<file>
# -DOUT_DIR=<dir> -P check_gen_csharp_constants.cmake

file(REMOVE_RECURSE "${OUT_DIR}")
file(MAKE_DIRECTORY "${OUT_DIR}")
execute_process(COMMAND "${ROBOTRACONTEURGEN}" --thunksource --lang=csharp "--output-dir=${OUT_DIR}" "${ROBDEF}"
                RESULT_VARIABLE res)
if(NOT res EQUAL 0)
    message(FATAL_ERROR "RobotRaconteurGen failed: ${res}")
endif()

file(READ "${OUT_DIR}/experimental__gen_test_java_constants.cs" src)
foreach(expected "public const float f1=1.5F;" "public static readonly float[] f2={1.5F, 2.5F, -3F};"
                 "public const ulong u5=18446744073709551615;")
    string(FIND "${src}" "${expected}" pos)
    if(pos EQUAL -1)
        message(FATAL_ERROR "Generated C# does not contain ${expected}")
    endif()
endforeach()

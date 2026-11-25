# Runs RobotRaconteurGen --lang=java and --lang=csharp on a robdef whose type and member names are Java / C# keywords,
# and checks that the generated code uses the names from the service definition (not the escaped identifiers) where a
# Robot Raconteur member or type name is expected. Usage: cmake -DROBOTRACONTEURGEN=<exe> -DROBDEF=<file>
# -DOUT_DIR=<dir> -P check_gen_names.cmake

file(REMOVE_RECURSE "${OUT_DIR}")
foreach(lang java csharp)
    file(MAKE_DIRECTORY "${OUT_DIR}/${lang}")
    execute_process(COMMAND "${ROBOTRACONTEURGEN}" --thunksource --lang=${lang} "--output-dir=${OUT_DIR}/${lang}"
                            "${ROBDEF}" RESULT_VARIABLE res)
    if(NOT res EQUAL 0)
        message(FATAL_ERROR "RobotRaconteurGen --lang=${lang} failed: ${res}")
    endif()
endforeach()

set(svc "experimental.gen_test_java_names")

file(READ "${OUT_DIR}/java/experimental/gen_test_java_names/o1_stub.java" java_stub)
foreach(
    expected
    "findObjRefTyped(\"goto\",String.valueOf(ind),\"${svc}.try\")" "findObjRefTyped(\"default\",\"${svc}.try\")"
    "asyncFindObjRefTyped(\"goto\",String.valueOf(ind),\"${svc}.try\",handler,timeout)"
    "asyncFindObjRefTyped(\"default\",\"${svc}.try\",handler,timeout)")
    string(FIND "${java_stub}" "${expected}" pos)
    if(pos EQUAL -1)
        message(FATAL_ERROR "Java stub does not contain ${expected}")
    endif()
endforeach()

file(READ "${OUT_DIR}/java/experimental/gen_test_java_names/native__stub.java" java_pod_stub)
string(FIND "${java_pod_stub}" "return \"${svc}.native\";" pos)
if(pos EQUAL -1)
    message(FATAL_ERROR "Java pod stub does not return the type name ${svc}.native")
endif()

file(GLOB cs_files "${OUT_DIR}/csharp/*.cs")
set(cs_src "")
foreach(f ${cs_files})
    file(READ "${f}" c)
    string(APPEND cs_src "${c}")
endforeach()
foreach(
    expected
    "FindObjRefTyped(\"goto\",ind.ToString(),\"${svc}.try\")" "FindObjRefTyped(\"default\",\"${svc}.try\")"
    "AsyncFindObjRefTyped<try_>(\"goto\",ind.ToString(),\"${svc}.try\",timeout)"
    "AsyncFindObjRefTyped<try_>(\"default\",\"${svc}.try\",timeout)")
    string(FIND "${cs_src}" "${expected}" pos)
    if(pos EQUAL -1)
        message(FATAL_ERROR "C# stub does not contain ${expected}")
    endif()
endforeach()

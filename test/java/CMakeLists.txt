if(NOT ANDROID)

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)

    if(NOT RobotRaconteur_GEN)
        set(RobotRaconteur_GEN RobotRaconteurGen)
    endif()

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_src")

    robotraconteur_generate_thunk(
        RR_THUNK_SRCS
        com.robotraconteur.testing.TestService1.robdef
        com.robotraconteur.testing.TestService2.robdef
        com.robotraconteur.testing.TestService3.robdef
        com.robotraconteur.testing.TestService4.robdef
        LANG
        java
        INCLUDE_DIRS
        "${CMAKE_SOURCE_DIR}/test/robdef"
        OUTDIR
        "${CMAKE_CURRENT_BINARY_DIR}/test_src")

    # Member and type names that are Java or C# keywords; compiled with the test to check the generated code
    robotraconteur_generate_thunk(
        RR_NAMES_THUNK_SRCS
        experimental.gen_test_java_names.robdef
        LANG
        java
        INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/robdef"
        OUTDIR
        "${CMAKE_CURRENT_BINARY_DIR}/test_src")

    # Containers of fixed size pod multidimensional arrays; compiled with the test to check the generated code
    robotraconteur_generate_thunk(
        RR_POD_CONTAINERS_THUNK_SRCS
        experimental.gen_test_java_pod_containers.robdef
        LANG
        java
        INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/robdef"
        OUTDIR
        "${CMAKE_CURRENT_BINARY_DIR}/test_src")

    # Numeric constants of all types; compiled and checked by the test
    robotraconteur_generate_thunk(
        RR_CONSTANTS_THUNK_SRCS
        experimental.gen_test_java_constants.robdef
        LANG
        java
        INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/robdef"
        OUTDIR
        "${CMAKE_CURRENT_BINARY_DIR}/test_src")

    add_custom_target(RobotRaconteurJavaTest_gen DEPENDS ${RR_THUNK_SRCS} ${RR_NAMES_THUNK_SRCS}
                                                         ${RR_POD_CONTAINERS_THUNK_SRCS} ${RR_CONSTANTS_THUNK_SRCS})

    # cSpell: disable
    add_custom_target(
        RobotRaconteurJavaTest ALL
        "${Java_JAVAC_EXECUTABLE}"
        -encoding
        UTF8
        -target
        8
        -source
        8
        -Xlint:none
        com/robotraconteur/javatest/*.java
        experimental/gen_test_java_constants/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService1/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService2/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService3/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/experimental/gen_test_java_names/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/experimental/gen_test_java_pod_containers/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/experimental/gen_test_java_constants/*.java
        -d
        "${CMAKE_CURRENT_BINARY_DIR}/test"
        -classpath
        "${CMAKE_BINARY_DIR}/RobotRaconteurJava/bin"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava RobotRaconteurJavaTest_gen)
    # cSpell: enable

    add_custom_command(
        TARGET RobotRaconteurJavaTest
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${ROBOTRACONTEUR_TEST_VECTORS_BIN}"
                "${CMAKE_CURRENT_BINARY_DIR}/test/test_vectors.bin"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/test/$<$<CONFIG:debug>:out_debug>$<$<CONFIG:release>:out>$<$<CONFIG:relwithdebinfo>:out_reldebug>/Java"
        COMMAND
            "${Java_JAR_EXECUTABLE}" cf
            "${CMAKE_BINARY_DIR}/test/$<$<CONFIG:debug>:out_debug>$<$<CONFIG:release>:out>$<$<CONFIG:relwithdebinfo>:out_reldebug>/Java/RobotRaconteurJavaTest.jar"
            *
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

    add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava)

    set(RR_JAVA_TEST_CMD "${Java_JAVA_EXECUTABLE}")
    if(WIN32)
        set(RR_JAVA_TEST_CMD_ARG
            "-cp ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteur.jar\\\;${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/Java/RobotRaconteurJavaTest.jar -Djava.library.path=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/ -XX:MaxJavaStackTraceDepth=100000 com.robotraconteur.javatest.RobotRaconteurJavaTest"
        )
    else()
        set(RR_JAVA_TEST_CMD_ARG
            "-cp ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteur.jar:${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/Java/RobotRaconteurJavaTest.jar -Djava.library.path=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/ -XX:MaxJavaStackTraceDepth=100000 com.robotraconteur.javatest.RobotRaconteurJavaTest"
        )
    endif()
    rrconfiguretest(
        test_prog_java
        "${RR_JAVA_TEST_CMD}"
        "test_prog"
        NOCTEST
        NODEBUG
        ARGS
        "${RR_JAVA_TEST_CMD_ARG}")
    rrconfiguretest(test_java_loopback "${RR_JAVA_TEST_CMD}" "test_prog" ARGS "${RR_JAVA_TEST_CMD_ARG} loopback")
    rrconfiguretest(test_java_loopback2 "${RR_JAVA_TEST_CMD}" "test_prog" ARGS "${RR_JAVA_TEST_CMD_ARG} loopback2")

    add_test(
        NAME RobotRaconteurGen.JavaCSharpKeywordNames
        COMMAND
            ${CMAKE_COMMAND} -DROBOTRACONTEURGEN=$<TARGET_FILE:RobotRaconteurGen>
            -DROBDEF=${CMAKE_CURRENT_SOURCE_DIR}/robdef/experimental.gen_test_java_names.robdef
            -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/gen_names_check -P ${CMAKE_CURRENT_SOURCE_DIR}/check_gen_names.cmake)

    add_test(
        NAME RobotRaconteurGen.CSharpNumericConstants
        COMMAND
            ${CMAKE_COMMAND} -DROBOTRACONTEURGEN=$<TARGET_FILE:RobotRaconteurGen>
            -DROBDEF=${CMAKE_CURRENT_SOURCE_DIR}/robdef/experimental.gen_test_java_constants.robdef
            -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/gen_constants_check -P
            ${CMAKE_CURRENT_SOURCE_DIR}/check_gen_csharp_constants.cmake)

endif()

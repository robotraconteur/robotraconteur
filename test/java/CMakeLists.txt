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

    add_custom_target(RobotRaconteurJavaTest_gen DEPENDS ${RR_THUNK_SRCS})

    add_custom_target(
        RobotRaconteurJavaTest ALL
        "${Java_JAVAC_EXECUTABLE}"
        -encoding
        UTF8
        -target
        7
        -source
        7
        -Xlint:none
        com/robotraconteur/javatest/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService1/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService2/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService3/*.java
        -d
        "${CMAKE_CURRENT_BINARY_DIR}/test"
        -classpath
        "${CMAKE_BINARY_DIR}/RobotRaconteurJava/bin"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava RobotRaconteurJavaTest_gen)

    add_custom_command(
        TARGET RobotRaconteurJavaTest
        POST_BUILD
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

endif()

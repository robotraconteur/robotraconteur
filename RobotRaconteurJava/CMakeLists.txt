file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/exceptionsrc)

if(NOT ANDROID)
    include(FindJava)
    find_package(Java COMPONENTS Development)
    include(UseJava)
    include(FindJNI)
    find_package(JNI)

    include_directories(${JNI_INCLUDE_DIRS})
else()
    include(FindJava)
    find_package(Java COMPONENTS Development)
endif()

if(ANDROID)
    set(LIBUSB_INCLUDE_DIR "" CACHE PATH "Path to libusb.h for android")
endif()

if(NOT USE_PREGENERATED_SOURCE)

    include(FindSWIG)
    find_package(SWIG 4.0.2 REQUIRED)
    include(${CMAKE_SOURCE_DIR}/cmake/RRUseSwig.cmake)

    include(JavaSwigVars.cmake)
    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/swigsrc/com/robotraconteur)
    include_directories(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include)
    rr_swig_add_module(
        RobotRaconteurJavaNative java RobotRaconteurJava.i ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h)

    swig_link_libraries(RobotRaconteurJavaNative RobotRaconteurCore)

else()
    set(SWIG_MODULE_RobotRaconteurJavaNative_REAL_NAME RobotRaconteurJavaNative)

    if("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Java/swigwordsize64/com/robotraconteur)
    else()
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Java/swigwordsize32/com/robotraconteur)
    endif()
    include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
    add_library(
        RobotRaconteurJavaNative MODULE
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        ${CMAKE_SWIG_OUTDIR}/RobotRaconteurJavaJAVA_wrap.cxx)
    target_link_libraries(RobotRaconteurJavaNative RobotRaconteurCore)

endif()

if(MSVC)
    target_compile_options(${SWIG_MODULE_RobotRaconteurJavaNative_REAL_NAME} PRIVATE "/bigobj")
endif()

target_compile_definitions(${SWIG_MODULE_RobotRaconteurJavaNative_REAL_NAME}
                           PRIVATE SWIG_TYPE_TABLE=robotraconteur_java)

if(APPLE)
    set_target_properties(RobotRaconteurJavaNative PROPERTIES SUFFIX ".jnilib")
endif()
if((WIN32 AND MINGW) OR CYGWIN OR CMAKE_SYSTEM_NAME STREQUAL MSYS)
    set_target_properties(RobotRaconteurJavaNative PROPERTIES PREFIX "")
endif()

if(NOT ANDROID)
    add_custom_target(
        RobotRaconteurJava ALL
        "${Java_JAVAC_EXECUTABLE}"
        -encoding
        UTF8
        -g:none
        -target
        7
        -source
        7
        -Xlint:none
        com/robotraconteur/*.java
        ${CMAKE_SWIG_OUTDIR}/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/exceptionsrc/com/robotraconteur/*.java
        -d
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_custom_command(
        TARGET RobotRaconteurJava
        POST_BUILD
        COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteur.jar" *
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
else()
    set(ANDROID_JAR "" CACHE FILEPATH "Android system Jar file")
    add_custom_target(
        RobotRaconteurJava ALL
        "${Java_JAVAC_EXECUTABLE}"
        -encoding
        UTF8
        -g:none
        -target
        7
        -source
        7
        -Xlint:none
        com/robotraconteur/*.java
        ${CMAKE_SWIG_OUTDIR}/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/exceptionsrc/com/robotraconteur/*.java
        -d
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        -cp
        "${ANDROID_JAR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_custom_command(
        TARGET RobotRaconteurJava
        POST_BUILD
        COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteurAndroid.jar" *
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()

include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)

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
    "${CMAKE_CURRENT_BINARY_DIR}/bin"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava RobotRaconteurJavaTest_gen)

add_custom_command(
    TARGET RobotRaconteurJavaTest
    POST_BUILD
    COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteurJavaTest.jar" *
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

rrsettargetdirs(RobotRaconteurJavaNative "Java" "Java")

set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative")

if(NOT DEFINED RobotRaconteur_DIR)
    add_dependencies(RobotRaconteurJavaNative RobotRaconteurCore)
endif()

add_dependencies(RobotRaconteurJava RobotRaconteurJavaNative)
add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-win64")
    else()
        set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-win32")
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    message(STATUS "Detected system processor ${CMAKE_SYSTEM_PROCESSOR}")

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm.*")
        # Assume hard float for arm
        set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux-armhf")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux-arm64")
    else()

        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux64")
        else()
            set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux32")
        endif()
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-darwin")

endif()

if(ANDROID)

    set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-android")

endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Manifest.txt.in" "${CMAKE_CURRENT_BINARY_DIR}/bin/Manifest.txt" @ONLY)

include("${CMAKE_CURRENT_SOURCE_DIR}/JavaExceptions.cmake")

set(RR_JAVA_TEST_CMD "${Java_JAVA_EXECUTABLE}")
if(WIN32)
    set(RR_JAVA_TEST_CMD_ARG
        "-cp ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteur.jar\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteurJavaTest.jar -Djava.library.path=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/ com.robotraconteur.javatest.RobotRaconteurJavaTest"
    )
else()
    set(RR_JAVA_TEST_CMD_ARG
        "-cp ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteur.jar:${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteurJavaTest.jar -Djava.library.path=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/ com.robotraconteur.javatest.RobotRaconteurJavaTest"
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




file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test)

IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_IMPORTS)
ENDIF()
ENDIF()

INCLUDE(FindSWIG)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(UseSWIG)


if (NOT(${__ANDROID__}))
INCLUDE(FindJava)
FIND_PACKAGE(Java COMPONENTS Development)
INCLUDE (UseJava)
INCLUDE (FindJNI)
FIND_PACKAGE(JNI)

include_directories(${JNI_INCLUDE_DIRS})
else()
INCLUDE(FindJava)
FIND_PACKAGE(Java COMPONENTS Development)
endif()

if (__ANDROID__)
set(GETIFADDR_SOURCE_FILE "" CACHE FILEPATH "Path to ifaddrs.c for android")
set(LIBUSB_INCLUDE_DIR "" CACHE PATH "Path to libusb.h for android")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-unused-function)
endif()

include(CheckTypeSize)

check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)
if ("${SWIG_JAVA_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX )
set(SWIG_JAVA_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
else()
set(SWIG_JAVA_EXTRA_ARGS "" CACHE STRING "Swig extra args")
endif()

include_directories("${CMAKE_SOURCE_DIR}/SWIG" "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/src")

INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)

set(Java_SWIG_sources Android.i DiscoveryJava.i HardwareTransportJava.i JavaExceptionTypemaps.i 
  JavaTypemaps.i MessageJava.i NodeIDjava.i RobotRaconteurJavaNode.i ServiceDefinitionJava.i 
  ServiceSecurityJava.i TcpTransportJava.i TimerJava.i TimeSpecJava.i)

set(SWIG_MODULE_RobotRaconteurJava_EXTRA_DEPS ${RobotRaconteur_SWIG_sources} ${Java_SWIG_sources})

SET(SWIG_CXX_EXTENSION cxx)
include_directories(${CMAKE_SOURCE_DIR}/SWIG)
set_property(SOURCE RobotRaconteurJava.i PROPERTY CPLUSPLUS ON)
if (${__ANDROID__})
set_property(SOURCE RobotRaconteurJava.i PROPERTY SWIG_FLAGS ${SWIG_JAVA_EXTRA_ARGS} -D__ANDROID__ -package com.robotraconteur )
else()
set_property(SOURCE RobotRaconteurJava.i PROPERTY SWIG_FLAGS ${SWIG_JAVA_EXTRA_ARGS} -package com.robotraconteur )
endif()
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/swigsrc/com/robotraconteur)
SWIG_ADD_MODULE(RobotRaconteurJavaNative java RobotRaconteurJava.i  ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h ${GETIFADDR_SOURCE_FILE} )

SWIG_LINK_LIBRARIES(RobotRaconteurJavaNative ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})

if (NOT(${__ANDROID__}))
add_custom_target(RobotRaconteurJava ALL "${Java_JAVAC_EXECUTABLE}" -encoding UTF8 -g:none -target 1.6 -source 1.6 -Xlint:none com/robotraconteur/*.java ${CMAKE_CURRENT_BINARY_DIR}/swigsrc/com/robotraconteur/*.java -d "${CMAKE_CURRENT_BINARY_DIR}/bin"  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_custom_command(TARGET RobotRaconteurJava POST_BUILD COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteur.jar" * WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
else()
set(ANDROID_JAR "" CACHE FILEPATH "Android system Jar file")
add_custom_target(RobotRaconteurJava ALL "${Java_JAVAC_EXECUTABLE}" -encoding UTF8 -g:none -target 1.6 -source 1.6 -Xlint:none com/robotraconteur/*.java ../androidsrc/com/robotraconteur/*.java ${CMAKE_CURRENT_BINARY_DIR}/swigsrc/com/robotraconteur/*.java -d "${CMAKE_CURRENT_BINARY_DIR}/bin" -cp "${ANDROID_JAR}"  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_custom_command(TARGET RobotRaconteurJava POST_BUILD COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteurAndroid.jar" * WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()


include(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/cmake/RobotRaconteurGenerateThunk.cmake)

if(NOT RobotRaconteur_GEN)
set(RobotRaconteur_GEN RobotRaconteurGen)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_src")

ROBOTRACONTEUR_GENERATE_THUNK(RR_THUNK_SRCS
	com.robotraconteur.testing.TestService1.robdef 
	com.robotraconteur.testing.TestService2.robdef 
	com.robotraconteur.testing.TestService3.robdef
	LANG java
	IMPORT_DIRS "${CMAKE_SOURCE_DIR}/testing/RobotRaconteurTest"
	OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/test_src"
	)

message(${RR_THUNK_SRCS})

add_custom_target(RobotRaconteurJavaTest_gen DEPENDS ${RR_THUNK_SRCS})

add_custom_target(RobotRaconteurJavaTest ALL "${Java_JAVAC_EXECUTABLE}" -encoding UTF8 -target 1.6 -source 1.6 -Xlint:none com/robotraconteur/JavaTest/*.java ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService1/*.java ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService2/*.java ${CMAKE_CURRENT_BINARY_DIR}/test_src/com/robotraconteur/testing/TestService3/*.java  -d "${CMAKE_CURRENT_BINARY_DIR}/test" -classpath "${CMAKE_CURRENT_BINARY_DIR}/bin"  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava RobotRaconteurJavaTest_gen)

add_custom_command(TARGET RobotRaconteurJavaTest POST_BUILD COMMAND "${Java_JAR_EXECUTABLE}" cf "$<TARGET_FILE_DIR:RobotRaconteurJavaNative>/RobotRaconteurJavaTest.jar" * WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

RRSetTargetDirs(RobotRaconteurJavaNative "Java" "Java")

set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative")

if (NOT DEFINED RobotRaconteur_DIR)
add_dependencies(RobotRaconteurJavaNative RobotRaconteurCore)
endif()

add_dependencies(RobotRaconteurJava RobotRaconteurJavaNative)
add_dependencies(RobotRaconteurJavaTest RobotRaconteurJava)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-win64")
else()
	set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-win32")
endif()
ENDIF()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux64")
else()
	set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-linux32")
endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	set_target_properties(RobotRaconteurJavaNative PROPERTIES OUTPUT_NAME "RobotRaconteurJavaNative-darwin")

endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Manifest.txt.in" "${CMAKE_CURRENT_BINARY_DIR}/bin/Manifest.txt" @ONLY)

include("${CMAKE_CURRENT_SOURCE_DIR}/JavaExceptions.cmake")

set(RR_JAVA_TEST_CMD " \"${Java_JAVA_EXECUTABLE}\" -cp ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteur.jar;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/RobotRaconteurJavaTest.jar -Djava.library.path=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Java/ com.robotraconteur.javatest.RobotRaconteurJavaTest")
RRConfigureTest(test_prog_java "${RR_JAVA_TEST_CMD}" "test_prog" NOCTEST NODEBUG)
RRConfigureTest(test_java_loopback "${RR_JAVA_TEST_CMD}" "test_prog" ARGS "loopback")
RRConfigureTest(test_java_loopback2 "${RR_JAVA_TEST_CMD}" "test_prog" ARGS "loopback2")

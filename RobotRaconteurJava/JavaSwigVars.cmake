if(NOT SWIG_JAVA_EXTRA_ARGS_OVERRIDE)
    if("${SWIG_JAVA_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(SWIG_JAVA_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
    else()
        set(SWIG_JAVA_EXTRA_ARGS "" CACHE STRING "Swig extra args")
    endif()
else()
    set(SWIG_JAVA_EXTRA_ARGS ${SWIG_JAVA_EXTRA_ARGS_OVERRIDE})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-unused-function)
endif()

include(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)
include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR} ${CMAKE_CURRENT_LIST_DIR}/swiglib_patched)

set(${CMAKE_CURRENT_LIST_DIR}/Java_SWIG_sources
    ${CMAKE_CURRENT_LIST_DIR}/Android.i
    ${CMAKE_CURRENT_LIST_DIR}/DiscoveryJava.i
    ${CMAKE_CURRENT_LIST_DIR}/HardwareTransportJava.i
    ${CMAKE_CURRENT_LIST_DIR}/JavaExceptionTypemaps.i
    ${CMAKE_CURRENT_LIST_DIR}/JavaTypemaps.i
    ${CMAKE_CURRENT_LIST_DIR}/MessageJava.i
    ${CMAKE_CURRENT_LIST_DIR}/NodeIDjava.i
    ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurJavaNode.i
    ${CMAKE_CURRENT_LIST_DIR}/ServiceDefinitionJava.i
    ${CMAKE_CURRENT_LIST_DIR}/ServiceSecurityJava.i
    ${CMAKE_CURRENT_LIST_DIR}/TcpTransportJava.i
    ${CMAKE_CURRENT_LIST_DIR}/TimerJava.i
    ${CMAKE_CURRENT_LIST_DIR}/TimeSpecJava.i)

set(SWIG_DEPENDS ${RobotRaconteur_SWIG_sources} ${Java_SWIG_sources})

set(SWIG_CXX_EXTENSION cxx)
set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurJava.i PROPERTY CPLUSPLUS ON)
if(ANDROID)
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurJava.i PROPERTY SWIG_FLAGS ${SWIG_JAVA_EXTRA_ARGS}
                                                                                -DANDROID -package com.robotraconteur)
else()
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurJava.i PROPERTY SWIG_FLAGS ${SWIG_JAVA_EXTRA_ARGS}
                                                                                -package com.robotraconteur)
endif()

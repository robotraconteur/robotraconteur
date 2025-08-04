# cSpell: ignore DSWIG

if(NOT SWIG_NET_EXTRA_ARGS_OVERRIDE)
    if("${SWIG_NET_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(SWIG_NET_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
    else()
        set(SWIG_NET_EXTRA_ARGS "" CACHE STRING "Swig extra args")
    endif()
else()
    set(SWIG_NET_EXTRA_ARGS ${SWIG_NET_EXTRA_ARGS_OVERRIDE})
endif()

include(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)
include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR} ${CMAKE_CURRENT_LIST_DIR}/swiglib_patched)

set(${CMAKE_CURRENT_LIST_DIR}/NET_SWIG_sources
    ${CMAKE_CURRENT_LIST_DIR}/DiscoveryNET.i
    ${CMAKE_CURRENT_LIST_DIR}/HardwareTransportNET.i
    ${CMAKE_CURRENT_LIST_DIR}/LocalTransportNET.i
    ${CMAKE_CURRENT_LIST_DIR}/NETExceptionTypemaps.i
    ${CMAKE_CURRENT_LIST_DIR}/NETTypemaps.i
    ${CMAKE_CURRENT_LIST_DIR}/MessageNET.i
    ${CMAKE_CURRENT_LIST_DIR}/NodeIDNET.i
    ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurNodeNET.i
    ${CMAKE_CURRENT_LIST_DIR}/ServiceSecurityNET.i
    ${CMAKE_CURRENT_LIST_DIR}/TcpTransportNET.i
    ${CMAKE_CURRENT_LIST_DIR}/TimerNET.i
    ${CMAKE_CURRENT_LIST_DIR}/TimeSpecNET.i)

set(SWIG_DEPENDS ${RobotRaconteur_SWIG_sources} ${NET_SWIG_sources})

set(SWIG_CXX_EXTENSION cxx)
set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurNET.i PROPERTY CPLUSPLUS ON)
set_property(
    SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurNET.i
    PROPERTY SWIG_FLAGS
             ${SWIG_NET_EXTRA_ARGS}
             -namespace
             RobotRaconteur
             -DSWIG2_CSHARP
             -outfile
             RobotRaconteurNET_SWIG.cs)
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

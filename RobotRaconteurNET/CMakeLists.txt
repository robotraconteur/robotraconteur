
IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_IMPORTS)
ENDIF()
ENDIF()

IF (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF()

if (NOT USE_PREGENERATED_SOURCE)

INCLUDE(FindSWIG)
FIND_PACKAGE(SWIG 4.0.0 REQUIRED)
INCLUDE(UseSWIG)

include(NETSwigVars.cmake)

SWIG_ADD_MODULE(RobotRaconteurNETNative csharp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h RobotRaconteurNET.i)

SWIG_LINK_LIBRARIES(RobotRaconteurNETNative ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})
else()
set(SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME RobotRaconteurNETNative)

if ("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize64/)
else()
set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize32/)
endif()
include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
add_library(RobotRaconteurNETNative MODULE ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h ${CMAKE_SWIG_OUTDIR}/RobotRaconteurNETCSHARP_wrap.cxx)
target_link_libraries(RobotRaconteurNETNative ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})
endif()
#if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET_wrap.cxx")

RRSetTargetDirs(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} "NET/Native" "NET/Native")


#get_target_property(outdir RobotRaconteurNETNative LIBRARY_OUTPUT_DIRECTORY)
#get_target_property(outdir RobotRaconteurNETNative RUNTIME_OUTPUT_DIRECTORY_DEBUG)


set(RR_NET_PLATFORM_SUFFIX FALSE CACHE BOOL "")
if(RR_NET_PLATFORM_SUFFIX)
set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative")
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}  PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-linux64")
else()
	set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}  PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-linux32")
endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}  PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-darwin")
endif()
else()
set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES PREFIX "${CMAKE_SHARED_MODULE_PREFIX}")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/NETExceptions.cmake")

if (MSVC)

set(RR32BIN "" CACHE PATH "32-bit build directory")
#add_custom_target(RobotRaconteurNET ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNET.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_EXTERNAL_MSPROJECT(RobotRaconteurNET ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj PLATFORM AnyCPU)
#add_custom_target(RobotRaconteurNETTest ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNETTest.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
else()
find_program(XBUILD_EXECUTABLE NAMES xbuild msbuild)
add_custom_target(RobotRaconteurNET ALL "${XBUILD_EXECUTABLE}" $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> RobotRaconteurNET.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
add_dependencies(RobotRaconteurNET RobotRaconteurNETNative)

include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)

if(NOT RobotRaconteur_GEN)
set(RobotRaconteur_GEN RobotRaconteurGen)
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

ROBOTRACONTEUR_GENERATE_THUNK(RR_THUNK_SRCS
	com.robotraconteur.testing.TestService1.robdef 
	com.robotraconteur.testing.TestService2.robdef 
	com.robotraconteur.testing.TestService3.robdef
	LANG csharp
	IMPORT_DIRS "${CMAKE_SOURCE_DIR}/testing/RobotRaconteurTest"
	OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/test"
	)

add_custom_target(RobotRaconteurNETTest_gen DEPENDS ${RR_THUNK_SRCS})
add_dependencies(RobotRaconteurNETNative RobotRaconteurNETTest_gen)
if(MSVC)
INCLUDE_EXTERNAL_MSPROJECT(RobotRaconteurNETTest ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj PLATFORM AnyCPU  RobotRaconteurNETTest_gen)
else()
find_program(XBUILD_EXECUTABLE NAMES xbuild msbuild)
add_custom_target(RobotRaconteurNETTest ALL "${XBUILD_EXECUTABLE}" $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> RobotRaconteurNETTest.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
add_dependencies(RobotRaconteurNETTest RobotRaconteurNET)
#add_custom_target(RobotRaconteurNETTestVB ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNETTestVB.vbproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
#add_dependencies(RobotRaconteurNETTestVB RobotRaconteurNET)

if (WIN32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(RR_PREFER_32BIT "<Prefer32Bit>false</Prefer32Bit>")
else()
set(RR_PREFER_32BIT "<Prefer32Bit>true</Prefer32Bit>")
endif()
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTest/RobotRaconteurNETTest.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj" @ONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Core/RobotRaconteurNET.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/Core/Lib/RobotRaconteurNET.csproj" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Core/RobotRaconteurNETTest.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/Core/Test/RobotRaconteurNETTest.csproj" @ONLY)

#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTestVB/RobotRaconteurNETTestVB.vbproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTestVB.vbproj" @ONLY)

STRING(TIMESTAMP CurrentYear "%Y")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Properties/AssemblyInfo.cs.in" "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs" @ONLY)


if(WIN32)
set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
set(RR_NET_TEST_CMD_ARGS "-E env PATH=\"%PATH%\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\" ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest")
else()
find_program(MONO_EXECUTABLE mono)
set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
set(RR_NET_TEST_CMD_ARGS "-E env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\ ${MONO_EXECUTABLE} ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest.exe")
endif()
RRConfigureTest(test_prog_net "${RR_NET_TEST_CMD}" "test_prog" NOCTEST ARGS "${RR_NET_TEST_CMD_ARGS}")
RRConfigureTest(test_net_loopback "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback")
RRConfigureTest(test_net_loopback2 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback2")

#endif()


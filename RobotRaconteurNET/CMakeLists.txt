
INCLUDE(FindSWIG)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(UseSWIG)

IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_IMPORTS)
ENDIF()
ENDIF()

include(CheckTypeSize)

check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)
if ("${SWIG_NET_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX )
set(SWIG_NET_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
else()
set(SWIG_NET_EXTRA_ARGS "" CACHE STRING "Swig extra args")
endif()

INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)

set(NET_SWIG_sources DiscoveryNET.i HardwareTransportNET.i LocalTransportNET.i
  NETExceptionTypemaps.i NETTypemaps.i MessageNET.i NodeIDNET.i RobotRaconteurNodeNET.i 
  ServiceSecurityNET.i TcpTransportNET.i TimerNET.i TimeSpecNET.i)

set(SWIG_MODULE_RobotRaconteurNETNative_EXTRA_DEPS ${RobotRaconteur_SWIG_sources} ${NET_SWIG_sources})

SET(SWIG_CXX_EXTENSION cxx)
include_directories(${CMAKE_SOURCE_DIR}/SWIG)
set_property(SOURCE RobotRaconteurNET.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE RobotRaconteurNET.i PROPERTY SWIG_FLAGS ${SWIG_NET_EXTRA_ARGS} -namespace RobotRaconteur -DSWIG2_CSHARP -outfile RobotRaconteurNET_SWIG.cs)
SWIG_ADD_MODULE(RobotRaconteurNETNative csharp  ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h RobotRaconteurNET.i)

SWIG_LINK_LIBRARIES(RobotRaconteurNETNative ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})

#if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET_wrap.cxx")

RRSetTargetDirs(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} "NET/Native" "NET/Native")

set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative")

#get_target_property(outdir RobotRaconteurNETNative LIBRARY_OUTPUT_DIRECTORY)
#get_target_property(outdir RobotRaconteurNETNative RUNTIME_OUTPUT_DIRECTORY_DEBUG)




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

set(NET_SN_KEY "" CACHE FILEPATH "SN key for NET assembly signing")
if (NET_SN_KEY)
get_filename_component(NET_SN_KEY_NAME ${NET_SN_KEY} NAME)
string(CONFIGURE "<PropertyGroup><AssemblyOriginatorKeyFile>@NET_SN_KEY@</AssemblyOriginatorKeyFile></PropertyGroup><ItemGroup><None Include=\"@NET_SN_KEY@\"><Link>@NET_SN_KEY_NAME@</Link></None></ItemGroup>" NET_SN_KEY_XML)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/NETExceptions.cmake")

if (MSVC)

set(RR32BIN "" CACHE PATH "32-bit build directory")
#add_custom_target(RobotRaconteurNET ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNET.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_EXTERNAL_MSPROJECT(RobotRaconteurNET ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj)
add_dependencies(RobotRaconteurNET RobotRaconteurNETNative)
#add_custom_target(RobotRaconteurNETTest ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNETTest.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

include(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/cmake/RobotRaconteurGenerateThunk.cmake)

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
INCLUDE_EXTERNAL_MSPROJECT(RobotRaconteurNETTest ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj  RobotRaconteurNETTest_gen)
add_dependencies(RobotRaconteurNETTest RobotRaconteurNET)
#add_custom_target(RobotRaconteurNETTestVB ALL msbuild -p:Configuration=$(Configuration) /t:rebuild RobotRaconteurNETTestVB.vbproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
#add_dependencies(RobotRaconteurNETTestVB RobotRaconteurNET)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET_embed.msbuild.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET_embed.msbuild" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTest/RobotRaconteurNETTest.csproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj" @ONLY)
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTestVB/RobotRaconteurNETTestVB.vbproj.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTestVB.vbproj" @ONLY)

STRING(TIMESTAMP CurrentYear "%Y")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Properties/AssemblyInfo.cs.in" "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs" @ONLY)



endif()

if(WIN32)
set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
set(RR_NET_TEST_CMD_ARGS "-E env PATH=\"%PATH%\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\" ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest")
RRConfigureTest(test_prog_net "${RR_NET_TEST_CMD}" "test_prog" NOCTEST ARGS "${RR_NET_TEST_CMD_ARGS}")
RRConfigureTest(test_net_loopback "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback")
RRConfigureTest(test_net_loopback2 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback2")
endif()

#endif()


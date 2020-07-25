if(NOT RobotRaconteur_GEN)
set(RobotRaconteur_GEN RobotRaconteurGen)
endif()

IF (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF()

include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)

ROBOTRACONTEUR_GENERATE_THUNK(RR_THUNK_SRCS RR_THUNK_HDRS 
	com.robotraconteur.testing.TestService1.robdef 
	com.robotraconteur.testing.TestService2.robdef 
	com.robotraconteur.testing.TestService3.robdef
	com.robotraconteur.testing.TestService4.robdef
	MASTER_HEADER
	)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(RobotRaconteurTest_src
	MessageSerializationTest.cpp
	MessageSerializationTest4.cpp
	MultiDimArrayTest.cpp
	RobotRaconteurTest.cpp	
	ServiceTest.cpp
	ServiceTestClient.cpp
	ServiceTest2.cpp
	ServiceTestClient2.cpp
	AsyncMessageTest.cpp
	CompareArray.cpp
	${RR_THUNK_SRCS}
)

set(RobotRaconteurTest_header
	MessageSerializationTest.h
	MessageSerializationTest4.h
	MultiDimArrayTest.h	
	ServiceTest.h
	ServiceTestClient.h
	ServiceTest2.h
	ServiceTestClient2.h
	AsyncMessageTest.h
	CompareArray.h
	${RR_THUNK_HDRS}
)

add_executable(RobotRaconteurTest ${RobotRaconteurTest_src} ${RobotRaconteurTest_header})

if(${CMAKE_VERSION} VERSION_GREATER "3.16.0") 
	target_precompile_headers(RobotRaconteurTest PRIVATE ${RR_THUNK_HDRS})
endif()

RRSetTargetDirs(RobotRaconteurTest "bin" "lib")

target_link_libraries(RobotRaconteurTest  ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})

if (NOT DEFINED RobotRaconteur_DIR)
add_dependencies(RobotRaconteurTest RobotRaconteurCore)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../testdata" DESTINATION "${CMAKE_BINARY_DIR}/out/")
#if (WIN32 OR (CMAKE_GENERATOR STREQUAL Xcode))
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../testdata" DESTINATION "${CMAKE_BINARY_DIR}/out_debug/")
#ENDIF()

RRConfigureTest(test_prog_cpp "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" NOCTEST)
RRConfigureTest(test_cpp_loopback "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" ARGS "loopback")
RRConfigureTest(test_cpp_loopback2 "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" ARGS "loopback2")
set(RR_ROBDEF_DIR "${CMAKE_SOURCE_DIR}/testing/RobotRaconteurTest")
RRConfigureTest(test_robdeftest "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" ARGS "robdeftest ${RR_ROBDEF_DIR}/com.robotraconteur.testing.TestService1.robdef ${RR_ROBDEF_DIR}/com.robotraconteur.testing.TestService2.robdef ${RR_ROBDEF_DIR}/com.robotraconteur.testing.TestService3.robdef")
RRConfigureTest(test_cpp_discoveryloopback "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" ARGS "discoveryloopback --robotraconteur-local-enable=false")

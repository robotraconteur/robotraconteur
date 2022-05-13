
include(${CMAKE_CURRENT_LIST_DIR}/cmake/macros.cmake)

if(NOT ROBOTRACONTEUR_GENERATE_THUNK)
include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)
endif()

ROBOTRACONTEUR_GENERATE_THUNK(RR_THUNK_SRCS RR_THUNK_HDRS 
	com.robotraconteur.testing.TestService1.robdef 
	com.robotraconteur.testing.TestService2.robdef 
	com.robotraconteur.testing.TestService3.robdef
	com.robotraconteur.testing.TestService4.robdef
	com.robotraconteur.testing.TestService5.robdef
	MASTER_HEADER
	INCLUDE_DIRS ${ROBOTRACONTEUR_TEST_ROBDEF_DIR}
	)

add_library(robotraconteur_test_service_test_lib STATIC ${RR_THUNK_SRCS})
target_link_libraries(robotraconteur_test_service_test_lib PUBLIC RobotRaconteurCore robotraconteur_test_cpp_common robotraconteur_test_lfsr GTest::Main)

RR_SERVICE_TEST_ADD_TEST(service SRC service_test.cpp ServiceTestClient.cpp CompareArray.cpp service_test_utils.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp ${RR_THUNK_SRCS})

RR_SERVICE_TEST_ADD_TEST(single_thread_service SRC single_thread_test.cpp ServiceTestClient.cpp CompareArray.cpp service_test_utils.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp ${RR_THUNK_SRCS})

RR_SERVICE_TEST_ADD_TEST(service2 SRC service_test2.cpp ServiceTestClient2.cpp ServiceTest2.cpp CompareArray.cpp service_test_utils.cpp ServiceTest.cpp ServiceTest3.cpp ${RR_THUNK_SRCS})

RR_SERVICE_TEST_ADD_TEST(service3 SRC service_test3.cpp ServiceTestClient3.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp ServiceTest.cpp ServiceTest2.cpp ${RR_THUNK_SRCS})

RR_SERVICE_TEST_ADD_EXE(server SRC test_server.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp)

RR_SERVICE_TEST_ADD_EXE(findservicebytype SRC findservicebytype.cpp)

RR_SERVICE_TEST_ADD_EXE(findnodebyname SRC findnodebyname.cpp)

RR_SERVICE_TEST_ADD_EXE(findnodebyid SRC findnodebyid.cpp)

RR_SERVICE_TEST_ADD_EXE(stresstestclient SRC stresstestclient.cpp)

RR_SERVICE_TEST_ADD_EXE(latencytestclient SRC latencytestclient.cpp)

RR_SERVICE_TEST_ADD_EXE(peeridentity SRC peeridentity.cpp)

RR_SERVICE_TEST_ADD_EXE(idleclient SRC idleclient.cpp)

RR_SERVICE_TEST_ADD_EXE(subscribertest SRC subscribertest.cpp)

RR_SERVICE_TEST_ADD_EXE(subscriberfiltertest SRC subscriberfiltertest.cpp)

RR_SERVICE_TEST_ADD_EXE(serviceinfo2subscribertest SRC serviceinfo2subscribertest.cpp)

RR_SERVICE_TEST_ADD_EXE(subscriberurltest SRC subscriberurltest.cpp)

RR_SERVICE_TEST_ADD_TEST(discovery_loopback SRC discovery_loopback.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp)

RR_SERVICE_TEST_ADD_TEST(intra_loopback SRC intra_loopback.cpp ServiceTestClient.cpp ServiceTestClient2.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp)

RR_SERVICE_TEST_ADD_EXE(certauthserver SRC certauthserver.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp)

RR_SERVICE_TEST_ADD_EXE(certauthclient SRC certauthclient.cpp ServiceTestClient.cpp ServiceTestClient2.cpp ServiceTest.cpp ServiceTest2.cpp ServiceTest3.cpp CompareArray.cpp service_test_utils.cpp)



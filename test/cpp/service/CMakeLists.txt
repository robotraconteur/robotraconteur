include(${CMAKE_CURRENT_LIST_DIR}/cmake/macros.cmake)

if(NOT ROBOTRACONTEUR_GENERATE_THUNK)
    include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    com.robotraconteur.testing.TestService1.robdef
    com.robotraconteur.testing.TestService2.robdef
    com.robotraconteur.testing.TestService3.robdef
    com.robotraconteur.testing.TestService4.robdef
    com.robotraconteur.testing.TestService5.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${ROBOTRACONTEUR_TEST_ROBDEF_DIR})

rr_service_test_add_lib(
    service_lib
    SRC
    ServiceTest.cpp
    ServiceTestClient.cpp
    ServiceTest2.cpp
    ServiceTestClient2.cpp
    ServiceTest3.cpp
    ServiceTestClient3.cpp
    CompareArray.cpp
    service_test_utils.cpp
    ${RR_THUNK_SRCS})

rr_service_test_add_test(service SRC service_test.cpp ${RR_THUNK_SRCS})

if(NOT (WIN32 AND BUILD_SHARED_LIBS))
    rr_service_test_add_test(single_thread_service SRC single_thread_test.cpp ${RR_THUNK_SRCS})
endif()

rr_service_test_add_test(service2 SRC service_test2.cpp ${RR_THUNK_SRCS})

rr_service_test_add_test(service3 SRC service_test3.cpp ${RR_THUNK_SRCS})

rr_service_test_add_exe(server SRC test_server.cpp service_test_utils.cpp)

rr_service_test_add_exe(findservicebytype SRC findservicebytype.cpp)

rr_service_test_add_exe(findnodebyname SRC findnodebyname.cpp)

rr_service_test_add_exe(findnodebyid SRC findnodebyid.cpp)

rr_service_test_add_exe(stresstestclient SRC stresstestclient.cpp)

rr_service_test_add_exe(latencytestclient SRC latencytestclient.cpp)

rr_service_test_add_exe(peeridentity SRC peeridentity.cpp)

rr_service_test_add_exe(idleclient SRC idleclient.cpp)

rr_service_test_add_exe(subscribertest SRC subscribertest.cpp)

rr_service_test_add_exe(subscriberfiltertest SRC subscriberfiltertest.cpp)

rr_service_test_add_exe(serviceinfo2subscribertest SRC serviceinfo2subscribertest.cpp)

rr_service_test_add_exe(subscriberurltest SRC subscriberurltest.cpp)

if(NOT ROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK)
    rr_service_test_add_test(discovery_loopback SRC discovery_loopback.cpp)
endif()

rr_service_test_add_test(intra_loopback SRC intra_loopback.cpp)

rr_service_test_add_exe(
    certauthserver
    SRC
    certauthserver.cpp
    ServiceTest.cpp
    ServiceTest2.cpp
    ServiceTest3.cpp
    CompareArray.cpp
    service_test_utils.cpp)

rr_service_test_add_exe(certauthclient SRC certauthclient.cpp)

rr_service_test_add_lib(
    server_lib
    SERVICELIB
    TYPE
    SHARED
    DEFINE
    ROBOTRACONTEUR_TEST_SERVER_LIB_EXPORTS
    SRC
    test_server_lib.cpp
    service_test_utils.cpp)

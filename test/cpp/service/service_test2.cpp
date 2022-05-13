#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTestClient2.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;


static std::string service2_url;


TEST(RobotRaconteurService,WirePeekPokeTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestWirePeekPoke());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,EnumsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestEnums());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,PodsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestPods());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,MemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,GeneratorsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestGenerators());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,NamedArraysTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNamedArrays());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,NamedArrayMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNamedArrayMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,ComplexTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestComplex());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,ComplexMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestComplexMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,NoLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNoLock());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,BoolsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestBools());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,BoolMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestBoolMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST(RobotRaconteurService,ExceptionParamsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestExceptionParams());
    ASSERT_NO_THROW(c.Disconnect());
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    TestServerNodeConfig server("unit_service_test2");
    service2_url = server.GetServiceURL("RobotRaconteurTestService2");

    ClientNodeSetup setup(ROBOTRACONTEUR_SERVICE_TYPES,argc,argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}
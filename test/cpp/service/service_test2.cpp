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

int my_argc;
char** my_argv;
static std::string service2_url;

RR_SHARED_PTR<TestServerNodeConfig> test_server_node_config;
RR_SHARED_PTR<ClientNodeSetup> client_node_setup;

class ServiceTest : public testing::Test
{
  protected:
    RR_OVIRTUAL void SetUp() RR_OVERRIDE
    {
        if (!test_server_node_config)
        {
            test_server_node_config = RR_MAKE_SHARED<TestServerNodeConfig>("unit_service_test");
            service2_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService2");
        }
        if (!client_node_setup)
        {
            client_node_setup = RR_MAKE_SHARED<ClientNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, my_argc, my_argv);
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, WirePeekPokeTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestWirePeekPoke());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, EnumsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestEnums());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, PodsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestPods());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, MemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, GeneratorsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestGenerators());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, NamedArraysTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNamedArrays());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, NamedArrayMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNamedArrayMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, ComplexTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestComplex());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, ComplexMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestComplexMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, NoLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestNoLock());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, BoolsTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestBools());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, BoolMemoriesTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient2 c;
    ASSERT_NO_THROW(c.Connect(service2_url));
    ASSERT_NO_THROW(c.TestBoolMemories());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, ExceptionParamsTest)
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
    my_argc = argc;
    my_argv = argv;

    int ret = RUN_ALL_TESTS();

    test_server_node_config.reset();
    client_node_setup.reset();

    return ret;
}
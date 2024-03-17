#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "ServiceTestClient.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

int my_argc;
char** my_argv;
static std::string service_url;
static std::string service_auth_url;

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
            service_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService");
            service_auth_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService_auth");
        }
        if (!client_node_setup)
        {
            client_node_setup = RR_MAKE_SHARED<ClientNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, my_argc, my_argv);
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, MinimalTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.RunMinimalTest(service_url));
}

TEST_F(ServiceTest, PropertyTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestProperties());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, FunctionTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestFunctions());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, EventTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestEvents());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, ObjRefTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestObjRefs());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, PipeTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestPipes());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, CallbackTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestCallbacks());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, WireTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestWires());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, MemoryTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.ConnectService(service_url));
    ASSERT_NO_THROW(c.TestMemories());
    ASSERT_NO_THROW(c.DisconnectService());
}

TEST_F(ServiceTest, AuthenticationTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestAuthentication(service_auth_url));
}

TEST_F(ServiceTest, ObjectLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestObjectLock(service_auth_url));
}

TEST_F(ServiceTest, MonitorLockTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestMonitorLock(service_url));
}

TEST_F(ServiceTest, AsyncTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient c;
    ASSERT_NO_THROW(c.TestAsync(service_auth_url));
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

#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTestClient3.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

int my_argc;
char** my_argv;
static std::string service3_url;

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
            service3_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService3");
        }
        if (!client_node_setup)
        {
            client_node_setup = RR_MAKE_SHARED<ClientNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, my_argc, my_argv);
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, Properties3Test)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient3 c;
    ASSERT_NO_THROW(c.Connect(service3_url));
    ASSERT_NO_THROW(c.TestProperties());
    ASSERT_NO_THROW(c.Disconnect());
}

TEST_F(ServiceTest, Functions3Test)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    ServiceTestClient3 c;
    ASSERT_NO_THROW(c.Connect(service3_url));
    ASSERT_NO_THROW(c.TestProperties());
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

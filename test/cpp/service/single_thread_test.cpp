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

RR_SHARED_PTR<TestServerNodeConfig> test_server_node_config;

static RR_BOOST_ASIO_IO_CONTEXT asio_io_context;

class ServiceTest : public testing::Test
{
  protected:
    RR_OVIRTUAL void SetUp() RR_OVERRIDE
    {
        if (!test_server_node_config)
        {
            test_server_node_config = RR_MAKE_SHARED<TestServerNodeConfig>("unit_service_test_st");
            service_url = test_server_node_config->GetServiceURL("RobotRaconteurTestService");
        }
    }

    RR_OVIRTUAL void TearDown() RR_OVERRIDE {}
};

TEST_F(ServiceTest, SingleThreadTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<IOContextThreadPool> thread_pool =
        RR_MAKE_SHARED<IOContextThreadPool>(RobotRaconteurNode::sp(), boost::ref(asio_io_context), false);

    RobotRaconteurNode::s()->SetThreadPool(thread_pool);

    ClientNodeSetup setup(ROBOTRACONTEUR_SERVICE_TYPES);

    ServiceTestClient cl;
    ASSERT_NO_THROW(cl.RunSingleThreadTest(service_url, asio_io_context));

    // Single thread does not require shutdown
    setup.ReleaseNode();
}

int main(int argc, char* argv[])
{

    testing::InitGoogleTest(&argc, argv);
    my_argc = argc;
    my_argv = argv;

    int ret = RUN_ALL_TESTS();

    test_server_node_config.reset();

    return ret;
}
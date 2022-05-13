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


static std::string service_url;


static RR_BOOST_ASIO_IO_CONTEXT asio_io_context;

TEST(RobotRaconteurServiceSingleThread,SingleThreadTest)
{
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();
    
    RR_SHARED_PTR<IOContextThreadPool> thread_pool = RR_MAKE_SHARED<IOContextThreadPool>(RobotRaconteurNode::sp(), boost::ref(asio_io_context), false);

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
    TestServerNodeConfig server("unit_service_test_st");
    service_url = server.GetServiceURL("RobotRaconteurTestService");

    int ret = RUN_ALL_TESTS();

    return ret;
}
#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTest.h"
#include "ServiceTest2.h"
#include "ServiceTest3.h"
#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
// using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

static RR_SHARED_PTR<TcpTransport> c;
int my_argc;
char** my_argv;

TEST(RobotRaconteurTestServer, Server)
{
    RR_SHARED_PTR<ServerNodeSetup> node_setup;
    try
    {
        node_setup = RR_MAKE_SHARED<ServerNodeSetup>(ROBOTRACONTEUR_SERVICE_TYPES, "testprog", 22222, my_argc, my_argv);
    }
    catch (std::exception&)
    {
        // A failed setup (e.g. the TCP port is in use) leaves the node running; shut it down before exiting
        if (detail::ThreadPool_IsNodeMultithreaded(RobotRaconteurNode::sp()))
        {
            RobotRaconteurNode::s()->Shutdown();
        }
        throw;
    }
    c = node_setup->GetTcpTransport();

    RobotRaconteurTestServiceSupport s;
    s.RegisterServices(c);

    RobotRaconteurTestService2Support s2;
    s2.RegisterServices(c);

    RobotRaconteurTestService3Support s3;
    s3.RegisterServices();

    std::cout << "Server started, press enter to quit" << std::endl;
    getchar();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    my_argc = argc;
    my_argv = argv;

    return RUN_ALL_TESTS();
}

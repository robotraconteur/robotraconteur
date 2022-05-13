#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "ServiceTest.h"
#include "ServiceTest2.h"
#include "ServiceTest3.h"
#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
//using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

static RR_SHARED_PTR<TcpTransport> c;

TEST(RobotRaconteurTestServer,Server)
{
    

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
    
    ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES,"testprog",22222,argc,argv);    
    c = node_setup.GetTcpTransport();

    return RUN_ALL_TESTS();
}
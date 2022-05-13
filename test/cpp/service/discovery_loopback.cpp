#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

#include "com__robotraconteur__testing__TestService1.h"
#include "com__robotraconteur__testing__TestService1_stubskel.h"

#include "ServiceTestClient.h"
#include "ServiceTest.h"
#include "robotraconteur_generated.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

TEST(RobotRaconteurService,DiscoveryLoopback)
{
    std::vector<std::string> args;
    ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "discovery_test_server_node", 0, args);

    RobotRaconteurTestServiceSupport s;
    s.RegisterServices(node_setup.GetTcpTransport());

    RR_SHARED_PTR<RobotRaconteurNode> client_node = RR_MAKE_SHARED<RobotRaconteurNode>();
    client_node->Init();
    ClientNodeSetup client_node_setup(client_node, ROBOTRACONTEUR_SERVICE_TYPES, args);

    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

    std::vector<std::string> schemes;
    if (node_setup.GetTcpTransport())
    {
        schemes.push_back("rr+tcp");
    }
    if (node_setup.GetLocalTransport())
    {
        schemes.push_back("rr+local");
    }
    BOOST_FOREACH (const std::string& scheme, schemes)
    {
        std::vector<std::string> schemes2;
        schemes2.push_back(scheme);
        std::vector<ServiceInfo2> discovered_services =
            client_node->FindServiceByType("com.robotraconteur.testing.TestService1.testroot", schemes2);

        std::cout << "Found " << discovered_services.size() << " services";

        std::set<std::string> expected_service_names;
        expected_service_names.insert("RobotRaconteurTestService");
        expected_service_names.insert("RobotRaconteurTestService_auth");

        EXPECT_EQ(discovered_services.size(),2);

        BOOST_FOREACH (ServiceInfo2 s, discovered_services)
        {
            EXPECT_EQ(expected_service_names.erase(s.Name), 1);
            EXPECT_EQ(s.NodeName, "discovery_test_server_node");
            EXPECT_EQ(s.RootObjectType, "com.robotraconteur.testing.TestService1.testroot");
            EXPECT_EQ(s.RootObjectImplements.size(), 1);
            EXPECT_EQ(s.RootObjectImplements.at(0), "com.robotraconteur.testing.TestService2.baseobj");
            if (s.Name == "RobotRaconteurTestService")
            {
                RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> c;
                ASSERT_NO_THROW(c =
                    rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                        client_node->ConnectService(s.ConnectionURL)));
                EXPECT_NO_THROW(c->get_d1());
                EXPECT_NO_THROW(client_node->DisconnectService(c));
            }
        }

        EXPECT_EQ(expected_service_names.size(),0);
    }
    
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    
    int ret = RUN_ALL_TESTS();

    return ret;
}
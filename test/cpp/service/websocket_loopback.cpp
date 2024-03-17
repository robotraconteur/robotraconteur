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

#include <boost/lexical_cast.hpp>

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;
using namespace RobotRaconteurTest;

TEST(RobotRaconteurService, WebsocketLoopback)
{
    RobotRaconteurNode::s()->SetNodeName("websocket_loopback");
    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<IntraTransport> c = RR_MAKE_SHARED<IntraTransport>();
    c->StartServer();

    RR_SHARED_PTR<TcpTransport> c2 = RR_MAKE_SHARED<TcpTransport>();
    c2->StartServer(0);

    // c->EnableNodeAnnounce();
    // c->EnableNodeDiscoveryListening();

    RobotRaconteurNode::s()->RegisterTransport(c);
    RobotRaconteurNode::s()->RegisterTransport(c2);
    RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    RobotRaconteurTestServiceSupport s;
    s.RegisterServices(c2);

    std::string port_str = boost::lexical_cast<std::string>(c2->GetListenPort());

    {
        ServiceTestClient cl;
        EXPECT_NO_THROW(
            cl.RunFullTest(std::string("rr+ws://localhost:") + port_str + "/?service=RobotRaconteurTestService",
                           std::string("rr+ws://localhost:") + port_str +
                               "/?nodename=websocket_loopback&service=RobotRaconteurTestService_auth"));
    }

    cout << "start shutdown" << endl;

    RobotRaconteurNode::s()->Shutdown();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}

#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for peeridentity:  peeridentity url [nodeid]" << endl;
        return -1;
    }

    string url1(argv[1]);

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();

    if (argc > 2)
    {
        std::string nodeid1 = (argv[2]);
        NodeID id(nodeid1);

        RobotRaconteurNode::s()->SetNodeID(NodeID(nodeid1));

        try
        {
            c->LoadTlsNodeCertificate();
        }
        catch (std::exception&)
        {
            std::cout << "warning: Could not load node certificate" << std::endl;
        }
    }

    c->EnableNodeAnnounce();
    c->EnableNodeDiscoveryListening();
    c->SetDefaultConnectTimeout(100000000);
    RobotRaconteurNode::s()->SetRequestTimeout(100000000);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();

    RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();

    RobotRaconteurNode::s()->RegisterTransport(c);
    RobotRaconteurNode::s()->RegisterTransport(c2);
    RobotRaconteurNode::s()->RegisterTransport(c5);
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

    RR_SHARED_PTR<RRObject> o = RobotRaconteurNode::s()->ConnectService(url1);

    RR_SHARED_PTR<testroot> oo = rr_cast<testroot>(o);

    oo->func3(1.0, 2.3);

    // RR_SHARED_PTR<Endpoint> ep = rr_cast<ServiceStub>(o)->GetContext();
    if (c->IsTransportConnectionSecure(o))
    {
        std::cout << "Connection is secure" << std::endl;
        if (c->IsSecurePeerIdentityVerified(o))
        {
            std::cout << "Peer identity is verified: " << c->GetSecurePeerIdentity(o) << std::endl;
        }
        else
        {
            std::cout << "Peer identity is not verified" << std::endl;
        }
    }
    else
    {
        std::cout << "Connection is not secure" << std::endl;
    }

    RobotRaconteurNode::s()->Shutdown();

    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    cout << "Test completed, no errors detected!" << endl;
    return 0;
}
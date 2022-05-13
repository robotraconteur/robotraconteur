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
        cout << "Usage for client:  idleclient url (seconds)" << endl;
        return -1;
    }

    string url1(argv[1]);
    int32_t seconds = 30;
    if (argc >= 4)
    {
        seconds = boost::lexical_cast<int32_t>(argv[3]);
    }
    // string url2(argv[3]);

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
    c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                            IPNodeDiscoveryFlags_SITE_LOCAL);
    c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                                    IPNodeDiscoveryFlags_SITE_LOCAL);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c2);
    c2->EnableNodeDiscoveryListening();

    RobotRaconteurNode::s()->RegisterTransport(c);
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    RR_SHARED_PTR<RRObject> obj = RobotRaconteurNode::s()->ConnectService(
        url1, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), RR_NULL_FN, "");
    RR_SHARED_PTR<testroot> o = rr_cast<testroot>(obj);
    o->get_d1();

    boost::this_thread::sleep(boost::posix_time::seconds(seconds));

    RobotRaconteurNode::s()->DisconnectService(obj);
    RobotRaconteurNode::s()->Shutdown();
    cout << "Test completed, no errors detected!" << endl;

    return 0;
}
#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage for client:  latencytestclient latencytestclient url url_auth" << endl;
        cout << "or:  latencytestclient latencytestclient2 url url_auth" << endl;
        return -1;
    }

    string command(argv[1]);

    string url1(argv[2]);
    // string url2(argv[3]);

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
    c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                            IPNodeDiscoveryFlags_SITE_LOCAL);
    c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                                    IPNodeDiscoveryFlags_SITE_LOCAL);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c2);

    RobotRaconteurNode::s()->RegisterTransport(c);
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    uint32_t servicetest_count = 0;
    uint32_t servicetest_connectcount = 0;
    bool servicetest_keepgoing = true;

    // RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue>
    // >(),NULL,"",&servicetest1);
    // RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue>
    // >(),NULL,"",boost::bind(&servicetest3,url1,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2)));

    RR_SHARED_PTR<RRObject> obj = RobotRaconteurNode::s()->ConnectService(
        url1, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), RR_NULL_FN, "");
    RR_SHARED_PTR<async_testroot> o = rr_cast<async_testroot>(obj);
    RR_SHARED_PTR<testroot> o2 = rr_cast<testroot>(obj);

    uint32_t iters = 100000;

    RR_SHARED_PTR<sub1> o3 = o2->get_o1();

    RR_INTRUSIVE_PTR<RRArray<double> > d = AllocateRRArray<double>(10);
    uint32_t latencyitercount = 0;
    RR_SHARED_PTR<AutoResetEvent> latency_event = RR_MAKE_SHARED<AutoResetEvent>();

    boost::posix_time::ptime t1;
    boost::posix_time::ptime t2;

    if (command == "latencytestclient")
    {
        t1 = RobotRaconteurNode::s()->NowNodeTime();
        for (uint32_t i = 0; i < iters; i++)
        {
            o3->set_d1(d);
        }
        t2 = RobotRaconteurNode::s()->NowNodeTime();
    }
    else
    {
        t1 = RobotRaconteurNode::s()->NowNodeTime();
        for (uint32_t i = 0; i < iters; i++)
        {
            o2->get_struct1();
        }
        t2 = RobotRaconteurNode::s()->NowNodeTime();
    }

    uint32_t diff = (uint32_t)(t2 - t1).total_microseconds();

    double period = ((double)diff) / ((double)iters);

    cout << "Period=" << period << endl;

    RobotRaconteurNode::s()->DisconnectService(obj);
    RobotRaconteurNode::s()->Shutdown();
    cout << "Test completed, no errors detected!" << endl;

    return 0;
}
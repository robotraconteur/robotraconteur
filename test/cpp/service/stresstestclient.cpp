#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;

uint64_t servicetest_connectcount;
bool servicetest_keepgoing;
uint64_t servicetest_count;

boost::mutex servicetest_lock;

void servicetest7(RR_SHARED_PTR<PipeEndpoint<double> > p)
{
    boost::mutex::scoped_lock lock(servicetest_lock);
    while (p->Available() > 0)
    {
        p->ReceivePacket();
    }
}

void servicetest6(uint32_t pnum, const RR_SHARED_PTR<RobotRaconteurException>& e) {}

void servicetest5(RR_SHARED_PTR<PipeEndpoint<double> > p, RR_SHARED_PTR<WireConnection<double> > w,
                  const TimerEvent& ev)
{
    if (ev.stopped)
        return;
    p->AsyncSendPacket((double)servicetest_count, &servicetest6);

    for (size_t i = 0; i < 100; i++)
    {
        double d = (double)servicetest_count * 100 + i;
        w->SetOutValue(d);
    }

    servicetest_count++;
}

void servicetest3(const std::string& url1, const RR_SHARED_PTR<RRObject>& obj,
                  const RR_SHARED_PTR<RobotRaconteurException>& exp);

void servicetest4(const std::string& url1)
{
    RobotRaconteurNode::s()->AsyncConnectService(
        url1, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), RR_NULL_FN, "",
        boost::bind(&servicetest3, url1, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
}

void servicetest3(const std::string& url1, const RR_SHARED_PTR<RRObject>& obj,
                  const RR_SHARED_PTR<RobotRaconteurException>& exp)
{
    if (exp)
    {
        cout << "Got exception" << endl;
        if (servicetest_keepgoing)
            std::terminate();
        return;
    }

    RobotRaconteurNode::s()->AsyncDisconnectService(obj, boost::bind(&servicetest4, url1));
}

void servicetest2(const RR_SHARED_PTR<async_testroot>& o, double d, const RR_SHARED_PTR<RobotRaconteurException>& exp)
{
    if (exp)
    {
        cout << "Got exception" << endl;
        if (servicetest_keepgoing)
            std::terminate();
        return;
    }

    o->async_func3(1, 2, boost::bind(&servicetest2, o, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
}

void servicetest1(const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& exp)
{
    if (exp)
    {
        cout << "Got exception" << endl;
        if (servicetest_keepgoing)
            std::terminate();
        return;
    }

    try
    {
        RR_SHARED_PTR<async_testroot> o = rr_cast<async_testroot>(obj);
        o->async_func3(1, 2, boost::bind(&servicetest2, o, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    }
    catch (std::exception&)
    {
        if (!servicetest_keepgoing)
            std::terminate();
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for client:  RobotRaconteurTest url" << endl;
        return -1;
    }

    string url1(argv[1]);
    // string url2(argv[3]);

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> c = RR_MAKE_SHARED<TcpTransport>();
    c->EnableNodeAnnounce(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                            IPNodeDiscoveryFlags_SITE_LOCAL);
    c->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                                    IPNodeDiscoveryFlags_SITE_LOCAL);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c2);

    RR_SHARED_PTR<HardwareTransport> c3 = RR_MAKE_SHARED<HardwareTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c3);

    RobotRaconteurNode::s()->RegisterTransport(c);
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    servicetest_count = 0;
    servicetest_connectcount = 0;
    servicetest_keepgoing = true;

    // RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue>
    // >(),NULL,"",&servicetest1);
    // RobotRaconteurNode::s()->AsyncConnectService(url1,"",RR_INTRUSIVE_PTR<RRMap<std::string,RRValue>
    // >(),NULL,"",boost::bind(&servicetest3,url1,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2)));

    RR_SHARED_PTR<RRObject> obj = RobotRaconteurNode::s()->ConnectService(
        url1, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), RR_NULL_FN, "");
    RR_SHARED_PTR<async_testroot> o = rr_cast<async_testroot>(obj);
    RR_SHARED_PTR<testroot> o2 = rr_cast<testroot>(obj);
    o->async_func3(1, 2, boost::bind(&servicetest2, o, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

    RR_SHARED_PTR<PipeEndpoint<double> > p = o2->get_broadcastpipe()->Connect(-1);
    p->PacketReceivedEvent.connect(&servicetest7);
    RR_SHARED_PTR<WireConnection<double> > w = o2->get_broadcastwire()->Connect();

    RR_SHARED_PTR<Timer> t = RobotRaconteurNode::s()->CreateTimer(
        boost::posix_time::milliseconds(40), boost::bind(&servicetest5, p, w, RR_BOOST_PLACEHOLDERS(_1)));
    t->Start();

    cout << "Press enter to quit" << endl;
    boost::this_thread::sleep(boost::posix_time::seconds(120));
    servicetest_keepgoing = false;
    t->Stop();
    RobotRaconteurNode::s()->DisconnectService(obj);
    RobotRaconteurNode::s()->Shutdown();
    cout << "Test completed, no errors detected!" << endl;
    boost::this_thread::sleep(boost::posix_time::seconds(10));    

    return 0;
}
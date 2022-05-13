#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;


// TODO: Use GTest

void servicesubscription_connected(const RR_SHARED_PTR<ServiceSubscription>& sub,
                                   const ServiceSubscriptionClientID& noden, const RR_SHARED_PTR<RRObject>& obj)
{
    cout << "Subscription Connected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void servicesubscription_disconnected(const RR_SHARED_PTR<ServiceSubscription>& sub,
                                      const ServiceSubscriptionClientID& noden, const RR_SHARED_PTR<RRObject>& obj)
{
    cout << "Subscription Disconnected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void subscribertest_waitwire(RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > w1)
{
    w1->WaitInValueValid();
    w1->WaitInValueValid();
}

void subscribertest_wirechanged(
    RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > w1,
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& val, const TimeSpec& time)
{
    // cout << "Wire value: " << RRArrayToScalar(val) << endl;
}

void subscribertest_pipereceived(RR_SHARED_PTR<PipeSubscription<double> > p1)
{
    static int c = 0;

    double val;
    while (p1->TryReceivePacket(val))
    {
        c++;
        if (c > 10)
        {
            cout << "Pipe value: " << val << endl;
            c = 0;
        }
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for subscribertest:  subscribertest servicetype" << endl;
        return -1;
    }

    string servicetype = string(argv[1]);
    // vector<string> schemes;
    // boost::split(schemes, argv[3], boost::is_from_range(',', ','));

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
    t->EnableNodeDiscoveryListening();
    RobotRaconteurNode::s()->RegisterTransport(t);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    c2->EnableNodeDiscoveryListening();
    RobotRaconteurNode::s()->RegisterTransport(c2);

    RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c5);

    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService1Factory>());
    RobotRaconteurNode::s()->RegisterServiceType(
        RR_MAKE_SHARED<com__robotraconteur__testing__TestService2Factory>());

    std::vector<std::string> servicetypes;
    servicetypes.push_back(servicetype);
    RR_SHARED_PTR<ServiceSubscription> subscription = RobotRaconteurNode::s()->SubscribeServiceByType(servicetypes);

    subscription->AddClientConnectListener(servicesubscription_connected);
    subscription->AddClientDisconnectListener(servicesubscription_disconnected);

    RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > broadcastwire =
        subscription->SubscribeWire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > >("broadcastwire");
    broadcastwire->AddWireValueChangedListener(subscribertest_wirechanged);

    RR_SHARED_PTR<WireSubscription<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > w1 =
        subscription->SubscribeWire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > >("w1");

    RR_SHARED_PTR<PipeSubscription<double> > broadcastpipe =
        subscription->SubscribePipe<double>("broadcastpipe", "", 100);
    broadcastpipe->AddPipePacketReceivedListener(subscribertest_pipereceived);

    boost::thread t1(boost::bind(&subscribertest_waitwire, w1));

    boost::this_thread::sleep(boost::posix_time::seconds(10));

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();
    typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
    BOOST_FOREACH (e_type& e, clients)
    {
        cout << "Subscribed Node: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
    }

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > w1_value;
    TimeSpec w1_time;
    broadcastwire->TryGetInValue(w1_value, &w1_time);

    double pipe_val;
    while (broadcastpipe->TryReceivePacket(pipe_val))
    {
        cout << "Got pipe value: " << pipe_val << endl;
    }

    if (w1_value)
    {
        cout << "broadcastwire: len=" << w1_value->size() << " [0]=" << (*w1_value)[0] << endl;
    }

    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > c;
    RR_INTRUSIVE_PTR<RRArray<double> > v;
    TimeSpec time;
    if (!broadcastwire->TryGetInValue(v, &time, &c))
    {
        cout << "Value not set" << endl;
    }

    cout << "Press enter to quit" << endl;

    getchar();

    subscription->Close();

    t1.join();

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}
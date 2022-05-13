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

static void asyncgetdefaultclient_handler(const RR_SHARED_PTR<testroot>& obj,
                                          const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    std::cout << "Got default client callback" << std::endl;
}

static void asyncgetdefaultclient_failed_handler(const RR_SHARED_PTR<ServiceSubscription>& sub,
                                                 const ServiceSubscriptionClientID& id,
                                                 const std::vector<std::string>& url,
                                                 const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    std::cout << "Connect to " << id.NodeID.ToString() << " with url: " << boost::join(url, ",")
              << " failed: " << err->ToString() << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for subscriberurltest:  RobotRaconteurTest subscriberurltest url" << endl;
        return -1;
    }

    string url = string(argv[1]);
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

    std::vector<std::string> urls;
    urls.push_back(url);
    RR_SHARED_PTR<ServiceSubscription> subscription = RobotRaconteurNode::s()->SubscribeService(urls);

    subscription->AddClientConnectListener(servicesubscription_connected);
    subscription->AddClientDisconnectListener(servicesubscription_disconnected);

    subscription->AddClientConnectFailedListener(boost::bind(&asyncgetdefaultclient_failed_handler,
                                                                RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                                RR_BOOST_PLACEHOLDERS(_3), RR_BOOST_PLACEHOLDERS(_4)));
    subscription->AsyncGetDefaultClient<testroot>(
        boost::bind(&asyncgetdefaultclient_handler, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)), 5000);
    RR_SHARED_PTR<testroot> defaultclient2 = subscription->GetDefaultClientWait<testroot>(10000);
    RR_SHARED_PTR<testroot> defaultclient3;
    subscription->TryGetDefaultClientWait<testroot>(defaultclient3, 10000);
    if (!defaultclient3)
    {
        throw std::runtime_error("");
    }

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();
    typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
    BOOST_FOREACH (e_type& e, clients)
    {
        cout << "Subscribed Node: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
    }

    try
    {
        cout << "Client d1: " << subscription->GetDefaultClient<testroot>()->get_d1() << endl;
    }
    catch (ConnectionException& e)
    {
        cout << "No client connected" << endl;
    }

    RR_SHARED_PTR<testroot> default_client;
    if (subscription->TryGetDefaultClient<testroot>(default_client))
    {
        cout << "Client d1: " << default_client->get_d1() << endl;
    }

    cout << "Press enter to quit" << endl;

    getchar();

    subscription->Close();

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}
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

bool subscribertest_filter_predicate(const ServiceInfo2& node)
{
    if (node.NodeName == "testprog")
    {
        return true;
    }

    return false;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        {
            cout << "Usage for subscriberfiltertest:  subscriberfiltertest servicetype" << endl;
            return -1;
        }

        string servicetype = string(argv[1]);
        // vector<string> schemes;
        // boost::split(schemes, argv[3], boost::is_from_range(',', ','));

        RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

        RR_SHARED_PTR<ServiceSubscriptionFilter> f = RR_MAKE_SHARED<ServiceSubscriptionFilter>();

        if (argc >= 3)
        {
        std::string subcommand = string(argv[2]);

        if (subcommand == "nodeid")
        {
            if (argc < 4)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest nodeid <nodeid>"
                        << endl;
                return -1;
            }

            RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
            n->NodeID = NodeID(std::string(argv[4]));
            f->Nodes.push_back(n);
        }
        else if (subcommand == "nodename")
        {
            if (argc < 4)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest nodename "
                        "<nodename>"
                        << endl;
                return -1;
            }

            RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
            n->NodeName = std::string(argv[3]);
            f->Nodes.push_back(n);
        }
        else if (subcommand == "nodeidscheme")
        {
            if (argc < 5)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest nodeidscheme "
                        "<nodeid> <schemes>"
                        << endl;
                return -1;
            }

            RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
            n->NodeID = NodeID(std::string(argv[3]));
            f->Nodes.push_back(n);
            std::vector<std::string> schemes;
            std::string schemes1(argv[4]);
            boost::split(schemes, schemes1, boost::is_any_of(","));
            f->TransportSchemes = schemes;
        }
        else if (subcommand == "nodeidauth")
        {
            if (argc < 6)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest nodeidauth "
                        "<nodeid> <username> <password>"
                        << endl;
                return -1;
            }

            RR_SHARED_PTR<ServiceSubscriptionFilterNode> n = RR_MAKE_SHARED<ServiceSubscriptionFilterNode>();
            n->NodeID = NodeID(std::string(argv[3]));
            n->Username = std::string(argv[4]);
            RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > cred = AllocateEmptyRRMap<std::string, RRValue>();
            cred->insert(std::make_pair("password", stringToRRArray(argv[5])));
            n->Credentials = cred;
            f->Nodes.push_back(n);
        }
        else if (subcommand == "servicename")
        {
            if (argc < 4)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest servicename "
                        "<servicename>"
                        << endl;
                return -1;
            }

            f->ServiceNames.push_back(argv[3]);
        }
        if (subcommand == "predicate")
        {
            if (argc < 3)
            {
                cout << "Usage for subscriberfiltertest:  subscriberfiltertest predicate"
                        << endl;
                return -1;
            }

            f->Predicate = subscribertest_filter_predicate;
        }
        else
        {
            cout << "Unknown subscriberfiltertest command" << endl;
            return -1;
        }
    }

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
    RR_SHARED_PTR<ServiceSubscription> subscription =
        RobotRaconteurNode::s()->SubscribeServiceByType(servicetypes, f);

    subscription->AddClientConnectListener(servicesubscription_connected);
    subscription->AddClientDisconnectListener(servicesubscription_disconnected);

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();
    typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;
    BOOST_FOREACH (e_type& e, clients)
    {
        cout << "Subscribed Node: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
    }

    cout << "Press enter to quit" << endl;

    getchar();

    subscription->Close();

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}
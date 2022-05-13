#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

using namespace RobotRaconteur;
using namespace std;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace com::robotraconteur::testing::TestService3;


// TODO: Use GTest

void serviceinfo2subscription_detected(const RR_SHARED_PTR<ServiceInfo2Subscription>& sub,
                                       const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
{
    cout << "ServiceInfo2 detected: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

void serviceinfo2subscription_lost(const RR_SHARED_PTR<ServiceInfo2Subscription>& sub,
                                   const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
{
    cout << "ServiceInfo2 lost: " << noden.NodeID.ToString() << ", " << noden.ServiceName << endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for serviceinfo2subscribertest:  serviceinfo2subscribertest servicetype"
                << endl;
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
    RR_SHARED_PTR<ServiceInfo2Subscription> subscription =
        RobotRaconteurNode::s()->SubscribeServiceInfo2(servicetypes);

    subscription->AddServiceDetectedListener(serviceinfo2subscription_detected);
    subscription->AddServiceLostListener(serviceinfo2subscription_lost);

    boost::this_thread::sleep(boost::posix_time::seconds(10));

    std::map<ServiceSubscriptionClientID, ServiceInfo2> clients = subscription->GetDetectedServiceInfo2();
    typedef std::map<ServiceSubscriptionClientID, ServiceInfo2>::value_type e_type;
    BOOST_FOREACH (e_type& e, clients)
    {
        cout << "Service Detected: " << e.first.NodeID.ToString() << " " << e.first.ServiceName << endl;
    }

    cout << "Press enter to quit" << endl;

    getchar();

    subscription->Close();

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}
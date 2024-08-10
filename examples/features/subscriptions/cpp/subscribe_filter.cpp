// subscribe_filter.cpp - Subscribe to a service by type using a filter

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;

bool subscription_predicate(const RR::ServiceInfo2& service_info2)
{
    // This function is called for each discovered service
    // Return true to connect to the service, false to ignore
    // The service_info2 parameter is a ServiceInfo2 object
    // containing information about the discovered service
    return true;
}

int main(int argc, char* argv[])
{
    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    // Populate a subscription filter. Note that all empty fields are considered "wildcards" that match any value
    auto f = RR_MAKE_SHARED<RR::ServiceSubscriptionFilter>();
    // Only connect to services named "reynard"
    f->ServiceNames.push_back("reynard");
    // Only use rr+tcp and rr+local transport
    f->TransportSchemes.push_back("rr+tcp");
    f->TransportSchemes.push_back("rr+local");
    // The ServiceSubscriptionFilter has powerful attribute matching capabilities
    // The use is intended for advanced use cases
    // f->Attributes = <Advanced attribute matching>
    // f->AttributesMatchOperation = RR::ServiceSubscriptionFilterAttributeGroupOperation_AND;
    // Set a "predicate" function that is called for each discovered service
    f->Predicate = subscription_predicate;
    // Set the maximum number of clients to connect. Default is -1 (no limit)
    f->MaxConnections = 5;

    // Create a ServiceSubscriptionFilterNode for node matching
    auto n = RR_MAKE_SHARED<RR::ServiceSubscriptionFilterNode>();
    // Only connect to nodes named "experimental.reynard_the_robot"
    n->NodeName = "experimental.reynard_the_robot";
    // Only connect to nodes with NodeID as listed. This will be different for every instance of the node
    // n->NodeID = RR::NodeID('12345678-1234-1234-1234-123456789abc');
    // Username and credentials can also be set for services that require authentication.
    // This should only be used when NodeID is also specified so the credentials are not sent to the wrong node
    // n->Username = "username";
    // auto credentials = RR::AllocateEmptyRRMap<std::string, RR::RRValue>();
    // credentials->insert({"password", RR::VarValue("my_password", "string")});
    // n->Credentials = credentials;
    f->Nodes.push_back(n);

    // Subscribe to a service by type using the filter
    std::vector<std::string> root_object_type = {"experimental.reynard_the_robot.Reynard"};
    RR::ServiceSubscriptionPtr sub = RR::RobotRaconteurNode::s()->SubscribeServiceByType(root_object_type, f);

    // Give a few seconds for the service to be connected
    boost::this_thread::sleep(boost::posix_time::seconds(6));

    // Run a function on all connected services. Could also use GetDefaultClientWait() to
    // get the first connected client and run functions on that client
    // without waiting for all clients to connect
    auto connected_clients = sub->GetConnectedClients();
    for (auto client : connected_clients)
    {
        auto reynard = RR::rr_cast<experimental::reynard_the_robot::Reynard>(client.second);
        reynard->say("Hello from subscribe_filter!");
    }

    return 0;
}

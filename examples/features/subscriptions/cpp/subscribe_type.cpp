// subscribe_type.cpp - Subscribe to a service by type

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;

int main(int argc, char* argv[])
{
    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    // Subscribe to a service by type
    std::vector<std::string> root_object_type = {"experimental.reynard_the_robot.Reynard"};
    RR::ServiceSubscriptionPtr sub = RR::RobotRaconteurNode::s()->SubscribeServiceByType(root_object_type);

    // Give a few seconds for the service to be connected
    boost::this_thread::sleep(boost::posix_time::seconds(6));

    // Run a function on all connected services. Could also use GetDefaultClientWait() to
    // get the first connected client and run functions on that client
    // without waiting for all clients to connect
    auto connected_clients = sub->GetConnectedClients();
    for (auto client : connected_clients)
    {
        auto reynard = RR::rr_cast<experimental::reynard_the_robot::Reynard>(client.second);
        reynard->say("Hello from subscribe_type!");
    }

    return 0;
}

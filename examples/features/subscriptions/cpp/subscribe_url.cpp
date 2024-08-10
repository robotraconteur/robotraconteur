// subscribe_url.cpp - Subscribe to a service using a URL

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

    // Subscribe to the service
    std::string url = "rr+tcp://localhost:29200/?service=reynard";
    RR::ServiceSubscriptionPtr sub = RR::RobotRaconteurNode::s()->SubscribeService(url);

    // Get the default client
    auto reynard = sub->GetDefaultClientWait<experimental::reynard_the_robot::Reynard>(10000);

    // Say hello
    reynard->say("Hello from subscribe_url!");

    return 0;
}

// connect_url.cpp - Connect to a service using a URL

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

    // Connect to the service
    std::string url = "rr+tcp://localhost:29200/?service=reynard";
    auto reynard =
        RR::rr_cast<experimental::reynard_the_robot::Reynard>(RR::RobotRaconteurNode::s()->ConnectService(url));

    // Say hello
    reynard->say("Hello from connect_url!");

    return 0;
}

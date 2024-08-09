// web_https_client.py - Example of connecting to a standard
// web server using HTTPS and WebSockets

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

    // Connect to the service using rr+wss, the secure WebSocket transport
    std::string url = "rr+wss://wstest2.wasontech.com/robotraconteur?service=testobj";
    auto c = RR::rr_cast<experimental::wstest::testobj>(RR::RobotRaconteurNode::s()->ConnectService(url));

    assert(c->add_two_numbers(2, 4) == 6);

    std::cout << "web_https_client example complete" << std::endl;

    return 0;
}

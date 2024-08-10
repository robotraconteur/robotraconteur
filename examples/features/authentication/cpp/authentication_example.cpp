// authentication_example.cpp - Example of using Robot Raconteur authentication

#include <stdio.h>
#include <iostream>
#include <boost/range/algorithm.hpp>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;

int main(int argc, char* argv[])
{
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    // Create the user credentials
    std::string username = "testuser1";
    RR::RRMapPtr<std::string, RR::RRValue> credentials = RR::AllocateEmptyRRMap<std::string, RR::RRValue>();
    credentials->insert(std::make_pair("password", RR::stringToRRArray("testpass1")));

    // Connect to the service using credentials
    experimental::reynard_the_robot::ReynardPtr c =
        RR::rr_cast<experimental::reynard_the_robot::Reynard>(RR::RobotRaconteurNode::s()->ConnectService(
            "rr+tcp://localhost:53226?service=authentication_example", username, credentials));

    // Use the service
    c->say("Message to secure service");

    std::cout << "authentication_example.cpp example complete" << std::endl;
    return 0;
}

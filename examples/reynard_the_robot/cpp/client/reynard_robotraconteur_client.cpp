#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;

#define M_PI 3.14159265358979323846

int main(int argc, char* argv[])
{
    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES);

    // Connect to the service
    std::string url = "rr+tcp://localhost:29200/?service=reynard";
    auto reynard =
        RR::rr_cast<experimental::reynard_the_robot::Reynard>(RR::RobotRaconteurNode::s()->ConnectService(url));

    // Connect a callback function to listen for new messages
    reynard->get_new_message().connect(
        [](const std::string& message) { std::cout << "New message: " << message << std::endl; });

    // Teleport the robot
    reynard->teleport(0.1, -0.2);

    // Drive the robot with no timeout
    reynard->drive_robot(0.5, -0.2, -1, false);

    // Wait for 1 second
    RR::RobotRaconteurNode::s()->Sleep(boost::posix_time::milliseconds(1000));

    // Stop the robot
    reynard->drive_robot(0, 0, -1, false);

    // Set the arm position
    reynard->setf_arm_position(100.0 * (M_PI / 180.0), -30.0 * (M_PI / 180.0), -70.0 * (M_PI / 180.0));

    // Drive the arm using timeout and wait
    reynard->drive_arm(10.0 * (M_PI / 180.0), -30.0 * (M_PI / 180.0), -15.0 * (M_PI / 180.0), 1.5, true);

    // Set the color to red
    double red_color[] = {1.0, 0.0, 0.0};
    RR::RRArrayPtr<double> color = RR::AttachRRArray<double>(red_color, 3, false);
    reynard->set_color(color);

    // Read the color
    auto color_in = reynard->get_color();
    std::cout << "Color: " << color_in->at(0) << " " << color_in->at(1) << " " << color_in->at(2) << std::endl;

    RR::RobotRaconteurNode::s()->Sleep(boost::posix_time::milliseconds(1000));

    // Reset the color
    double reset_color_a[] = {0.929, 0.49, 0.129};
    RR::RRArrayPtr<double> reset_color = RR::AttachRRArray<double>(reset_color_a, 3, false);
    reynard->set_color(reset_color);

    // Say hello
    reynard->say("Hello, World From C++!");

    // Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
    RR::TimeSpec ts;
    experimental::reynard_the_robot::ReynardStatePtr state = reynard->get_state()->PeekInValue(ts);
    std::cout << "State: " << state->robot_position->at(0) << " " << state->robot_position->at(0) << std::endl;

    return 0;
}

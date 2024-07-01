#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include <RobotRaconteurCompanion/StdRobDef/StdRobDefAll.h>
#include <RobotRaconteurCompanion/Converters/EigenConverters.h>

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace robot = com::robotraconteur::robotics::robot;
namespace RRC_Eigen = RobotRaconteur::Companion::Converters::Eigen;

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:52511?service=robot";
    if (argc >= 2)
    {
        url = argv[1];
    }

    // Register the standard Robot Raconteur types
    RobotRaconteur::Companion::RegisterStdRobDefServiceTypes();

    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    // In this example we only use standard types so no generated types are required
    RR::ClientNodeSetup node_setup(std::vector<RR::ServiceFactoryPtr>(), argc, argv);

    // Connect to the robot driver
    auto c = RR::rr_cast<robot::Robot>(RR::RobotRaconteurNode::s()->ConnectService(url));

    // Get the current state and print command mode to the console
    RR::TimeSpec state_ts;
    auto state = c->get_robot_state()->PeekInValue(state_ts);
    std::cout << "Command mode: " << state->command_mode << std::endl;

    // Change the robot command mode, first to halt, then to jog
    c->set_command_mode(robot::RobotCommandMode::halt);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    c->set_command_mode(robot::RobotCommandMode::jog);

    auto start_joint_pos = RRC_Eigen::RRArrayToEigen<Eigen::VectorXd>(state->joint_position);

    auto t_start = boost::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++)
    {
        // Get the current time in seconds
        auto t = (boost::chrono::high_resolution_clock::now() - t_start).count() / 1e9;

        // Generate the new command vector
        Eigen::VectorXd cmd = Eigen::VectorXd::Zero(6);
        cmd(0) = 0.2 * sin(t / 5.0);
        cmd += start_joint_pos;

        Eigen::VectorXd vel = Eigen::VectorXd::Ones(6);

        // Send the new command vector to the robot
        c->jog_freespace(RRC_Eigen::EigenToRRArray(cmd), RRC_Eigen::EigenToRRArray(vel), true);

        // Peek the current state flags
        state = c->get_robot_state()->PeekInValue(state_ts);
        std::cout << state->robot_state_flags << std::endl;

        // Wait for the next loop iteration
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    return 0;
}

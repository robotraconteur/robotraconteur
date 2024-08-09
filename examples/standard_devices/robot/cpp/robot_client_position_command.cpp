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

    // Change the robot command mode, first to halt, then to position_command
    c->set_command_mode(robot::RobotCommandMode::halt);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    c->set_command_mode(robot::RobotCommandMode::position_command);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    // Connect to the position_command and robot_state wires for real-time data streaming
    auto cmd_w = c->get_position_command()->Connect();
    auto state_w = c->get_robot_state()->Connect();
    // Set a lifespan of 500 ms for the robot state. If new packets
    // are not received within 500 ms, an exception will be thrown
    state_w->SetInValueLifespan(500);

    // Wait for the state_w wire to receive valid data
    state_w->WaitInValueValid();

    // Initialize command_seqno. command_seqno must increment for each position command
    uint32_t command_seqno = 0;

    // Retrieve the current joint position
    auto start_joint_pos = RRC_Eigen::RRArrayToEigen<Eigen::VectorXd>(state_w->GetInValue()->joint_position);

    auto t_start = boost::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        // Get the current time in seconds
        auto t = (boost::chrono::high_resolution_clock::now() - t_start).count() / 1e9;

        // Retrieve the current robot state
        auto robot_state = state_w->GetInValue();

        // Make sure the robot is still in position mode
        if (robot_state->command_mode != robot::RobotCommandMode::position_command)
        {
            std::cerr << "Robot is not in position mode" << std::endl;
            return 1;
        }

        // Increment command_seqno
        command_seqno++;

        // Create and fill the RobotJointCommand structure
        robot::RobotJointCommandPtr joint_cmd1(new robot::RobotJointCommand());
        joint_cmd1->seqno = command_seqno;            // Strictly increasing command_seqno
        joint_cmd1->state_seqno = robot_state->seqno; // Send current robot_state.seqno as failsafe

        // Generate the new command vector
        Eigen::VectorXd cmd = Eigen::VectorXd::Zero(6);
        cmd(0) = 0.2 * sin(t);
        cmd += start_joint_pos;

        // Set the joint command
        joint_cmd1->command = RRC_Eigen::EigenToRRArray(cmd);
        joint_cmd1->units = RR::AllocateRRArray<uint8_t>(0);

        // Send the joint command to the robot
        cmd_w->SetOutValue(joint_cmd1);

        // Wait for the next loop iteration
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }

    return 0;
}

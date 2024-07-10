#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include <RobotRaconteurCompanion/StdRobDef/StdRobDefAll.h>
#include <RobotRaconteurCompanion/Converters/EigenConverters.h>

// Only use namespace aliases in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace robot = com::robotraconteur::robotics::robot;
namespace trajectory = com::robotraconteur::robotics::trajectory;
namespace RRC_Eigen = RobotRaconteur::Companion::Converters::Eigen;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:52511?service=robot";
    if (argc >= 3)
    {
        url = argv[2];
    }

    if (argc < 2)
    {
        std::cout << "Robot type must be specified" << std::endl;
        return 1;
    }

    std::string robot_type = argv[1];

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

    // Change the robot command mode, first to halt, then to trajectory
    c->set_command_mode(robot::RobotCommandMode::halt);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    c->set_command_mode(robot::RobotCommandMode::trajectory);

    // Get the joint names and fill the joint_names list
    auto robot_info = c->get_robot_info();
    RR::RRListPtr<RR::RRArray<char> > joint_names = RR::AllocateEmptyRRList<RR::RRArray<char> >();
    for (auto& j : *robot_info->joint_info)
    {
        joint_names->push_back(RR::stringToRRArray(j->joint_identifier->name));
    }

    // Get the initial robot position and target position
    state = c->get_robot_state()->PeekInValue(state_ts);
    auto j_start = RRC_Eigen::RRArrayToEigen<Eigen::VectorXd>(state->joint_position);
    Eigen::VectorXd j_end(6);

    if (robot_type == "abb")
    {
        j_end << 0.0, -0.1, 0.25, 0.0, 0.0, 0.0;
    }
    else if (robot_type == "ur")
    {
        j_end << 0.0, -1.0, -1.5, 0.0, 0.0, 0.0;
    }
    else
    {
        std::cout << "Unknown robot type" << std::endl;
        return 1;
    }

    // Build up JointTrajectoryWaypoint(s) to move the robot to specified joint angles
    RR::RRListPtr<trajectory::JointTrajectoryWaypoint> waypoints =
        RR::AllocateEmptyRRList<trajectory::JointTrajectoryWaypoint>();

    for (double i = 0; i < 251; i++)
    {
        trajectory::JointTrajectoryWaypointPtr wp(new trajectory::JointTrajectoryWaypoint());
        Eigen::VectorXd j = j_start + (j_end - j_start) * i / 250.0;
        wp->time_from_start = i / 25.0;
        wp->joint_position = RRC_Eigen::EigenToRRArray(j);
        wp->joint_velocity = RR::AllocateRRArray<double>(0);
        wp->position_tolerance = RR::AllocateRRArray<double>(0);
        wp->velocity_tolerance = RR::AllocateRRArray<double>(0);
        waypoints->push_back(wp);
    }

    trajectory::JointTrajectoryPtr traj(new trajectory::JointTrajectory());
    traj->joint_names = joint_names;
    traj->waypoints = waypoints;

    c->set_speed_ratio(1.0);

    auto traj_gen = c->execute_trajectory(traj);

    bool res = true;

    do
    {
        trajectory::TrajectoryStatusPtr status;
        res = traj_gen->TryNext(status);

    } while (res);

    // Execute more trajectories. Trajectories are queued if executed concurrently

    RR::RRListPtr<trajectory::JointTrajectoryWaypoint> waypoints2 =
        RR::AllocateEmptyRRList<trajectory::JointTrajectoryWaypoint>();

    for (double i = 0; i < 251; i++)
    {
        double t = i / 25.0;

        trajectory::JointTrajectoryWaypointPtr wp(new trajectory::JointTrajectoryWaypoint());
        Eigen::VectorXd cmd(6);
        cmd << 1.0, 0.0, 0.0, 0.0, 0.5, -1.0;
        cmd *= 15 * (M_PI / 180.0) * sin(2.0 * M_PI * (t / 10.0));
        cmd += j_end;
        wp->time_from_start = t;
        wp->joint_position = RRC_Eigen::EigenToRRArray(cmd);
        wp->joint_velocity = RR::AllocateRRArray<double>(0);
        wp->position_tolerance = RR::AllocateRRArray<double>(0);
        wp->velocity_tolerance = RR::AllocateRRArray<double>(0);
        waypoints2->push_back(wp);
    }

    trajectory::JointTrajectoryPtr traj2(new trajectory::JointTrajectory());
    traj2->joint_names = joint_names;
    traj2->waypoints = waypoints2;

    c->set_speed_ratio(0.5);

    auto traj_gen2 = c->execute_trajectory(traj2);
    trajectory::TrajectoryStatusPtr status2;
    traj_gen2->TryNext(status2);

    c->set_speed_ratio(2.0);
    auto traj_gen3 = c->execute_trajectory(traj2);
    traj_gen3->TryNext(status2);

    do
    {
        trajectory::TrajectoryStatusPtr status;
        res = traj_gen2->TryNext(status);

    } while (res);

    do
    {
        trajectory::TrajectoryStatusPtr status;
        res = traj_gen3->TryNext(status);

    } while (res);

    return 0;
}

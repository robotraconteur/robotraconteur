using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;
using com.robotraconteur.robotics.robot;
using com.robotraconteur.robotics.trajectory;
using System.Diagnostics;
using System.Collections.Generic;

// URL for connecting to the robot. By default connects to UR5e simulated robot.
string url = "rr+tcp://localhost:52511?service=robot";
if (args.Length >= 3)
{
    url = args[2];
}

string robot_type = "ur"; // Set the robot type to ur by default
if (args.Length >= 2)
{
    robot_type = args[1];
}

// Initialize the client node
using (var node_setup = new ClientNodeSetup())
{
    // Connect to the robot
    var c = (Robot)RobotRaconteurNode.s.ConnectService(url);

    // Get the joint names and fill the joint_names list
    var robot_info = c.robot_info;
    List<string> joint_names = robot_info.joint_info.Select(x => x.joint_identifier.name).ToList();

    // Retrieve the current robot state and print the current command mode
    var robot_state = c.robot_state.PeekInValue(out var state_ts);
    Console.WriteLine(robot_state.command_mode);

    // Change the robot command mode, first to halt, then to trajectory
    c.command_mode = RobotCommandMode.halt;
    Thread.Sleep(100);
    c.command_mode = RobotCommandMode.trajectory;

    // Get the starting joint positions
    var robot_state2 = c.robot_state.PeekInValue(out var state_ts2);
    var j_start = robot_state2.joint_position;

    double[] j_end;

    if (robot_type == "abb")
    {
        j_end = new double[] { 0, -0.1, 0.25, 0, 0, 0 };
    }
    else if (robot_type == "ur")
    {
        j_end = new double[] { 0, -1, -1.5, 0, 0, 0 };
    }
    else
    {
        throw new ArgumentException("Invalid robot type");
    }

    var waypoints = new List<JointTrajectoryWaypoint>();

    for (double i = 0; i < 251; i++)
    {
        var wp = new JointTrajectoryWaypoint();
        var cmd = new double[6];
        for (int j = 0; j < 6; j++)
        {
            cmd[j] = j_start[j] + (j_end[j] - j_start[j]) * i / 250.0;
        }
        wp.time_from_start = i / 25.0;
        wp.joint_position = cmd;
        wp.joint_velocity = new double[0];
        wp.position_tolerance = new double[0];
        wp.velocity_tolerance = new double[0];
        waypoints.Add(wp);
    }

    var traj = new JointTrajectory();
    traj.joint_names = joint_names;
    traj.waypoints = waypoints;

    c.speed_ratio = 1.0;

    var traj_gen = c.execute_trajectory(traj);

    bool res = true;

    do
    {
        res = traj_gen.TryNext(out var traj_status);

    } while (res);
}

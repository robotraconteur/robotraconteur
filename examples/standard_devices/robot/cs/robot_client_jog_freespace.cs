using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;
using com.robotraconteur.robotics.robot;
using com.robotraconteur.robotics.trajectory;
using System.Diagnostics;

// URL for connecting to the robot. By default connects to UR5e simulated robot.
string url = "rr+tcp://localhost:52511?service=robot";
if (args.Length >= 2)
{
    url = args[1];
}

// Initialize the client node
using (var node_setup = new ClientNodeSetup())
{
    // Connect to the robot
    var c = (Robot)RobotRaconteurNode.s.ConnectService(url);

    // Retrieve the current robot state and print the current command mode
    var robot_state = c.robot_state.PeekInValue(out var state_ts);
    Console.WriteLine(robot_state.command_mode);

    // Change the robot command mode, first to halt, then to jog
    c.command_mode = RobotCommandMode.halt;
    Thread.Sleep(100);
    c.command_mode = RobotCommandMode.jog;

    // Get the starting joint positions
    var robot_state2 = c.robot_state.PeekInValue(out var state_ts2);
    var start_joint_pos = robot_state2.joint_position;

    var sw = new Stopwatch();
    sw.Start();

    TimeSpan t_start = sw.Elapsed;

    // Move the robot in a loop
    for (int i = 0; i < 100; i++)
    {
        var t = (double)((sw.Elapsed - t_start).TotalMilliseconds) / 1000.0;

        // Generate a sin wave
        var cmd = new double[6];
        for (int j = 0; j < 6; j++)
        {
            cmd[j] = start_joint_pos[j];
        }
        cmd[0] += 0.1 * Math.Sin(t / 5.0);

        // Send the command to the robot
        c.jog_freespace(cmd, new double[6] { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }, true);

        // Wait for the next loop iteration
        Thread.Sleep(100);
    }
}

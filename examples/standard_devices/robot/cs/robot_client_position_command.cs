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

    // Change the robot command mode, first to halt, then to position_command
    c.command_mode = RobotCommandMode.halt;
    Thread.Sleep(100);
    c.command_mode = RobotCommandMode.position_command;
    Thread.Sleep(100);

    // Connect to the position_command and robot_state wires for real-time data streaming
    var cmd_w = c.position_command.Connect();
    var state_w = c.robot_state.Connect();
    // Set a lifespan of 500 ms for the robot state. If new packets
    // are not received within 500 ms, an exception will be thrown
    state_w.InValueLifespan = 500;

    // Wait for the state_w wire to receive valid data
    state_w.WaitInValueValid();

    // Initialize command_seqno. command_seqno must increment for each position command
    ulong command_seqno = 1;

    // Retrieve the current joint position
    var init_joint_pos = state_w.InValue.joint_position;

    var sw = new Stopwatch();
    sw.Start();

    TimeSpan t_start = sw.Elapsed;

    // Move the robot in a loop
    for (int i = 0; i < 2000; i++)
    {
        var t = (double)((sw.Elapsed - t_start).TotalMilliseconds) / 1000.0;

        // Retreive the current robot state
        robot_state = state_w.InValue;

        // Make sure the robot is still in position mode
        if (robot_state.command_mode != RobotCommandMode.position_command)
        {
            throw new Exception("Robot is not in position mode");
        }

        // Increment command_seqno
        command_seqno += 1;

        // Create Fill the RobotJointCommand structure
        var joint_cmd1 = new RobotJointCommand();
        joint_cmd1.seqno = command_seqno;           // Strictly increasing command_seqno
        joint_cmd1.state_seqno = robot_state.seqno; // Send current robot_state.seqno as failsafe

        // Generate a joint command, in this case a sin wave
        var cmd = new double[6];
        for (int j = 0; j < 6; j++)
        {
            cmd[j] = init_joint_pos[j];
        }
        cmd[0] += 0.2 * Math.Sin(t);

        // Set the joint command
        joint_cmd1.command = cmd;
        joint_cmd1.units = new byte[0];

        // Send the command to the robot
        cmd_w.OutValue = joint_cmd1;

        // Wait for the next loop iteration
        Thread.Sleep(10);
    }
}

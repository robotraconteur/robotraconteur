using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;

// Initialize the client node
using (var node_setup = new ClientNodeSetup())
{
    // Connect to the Reynard service using a URL
    var c = (experimental.reynard_the_robot.Reynard)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:29200?service=reynard");

    // Connect a callback function to listen for new messages
    c.new_message += (msg) =>
    { Console.WriteLine(msg); };

    // Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
    var state = c.state.PeekInValue(out var state_ts);
    Console.WriteLine(string.Join(",", state.robot_position.Select(x => x.ToString())));
    Console.WriteLine(string.Join(",", state.arm_position.Select(x => x.ToString())));

    // Teleport the robot
    c.teleport(0.1, -0.2);

    // Drive the robot with no timeout
    c.drive_robot(0.5, -0.2, -1, false);

    // Wait for one second
    RobotRaconteurNode.s.Sleep(1000);

    // Stop the robot
    c.drive_robot(0, 0, -1, false);

    // Set the arm position
    c.setf_arm_position(100.0 * (Math.PI / 180), -30 * (Math.PI / 180), -70 * (Math.PI / 180));

    // Drive the arm using timeout and wait
    c.drive_arm(10.0 * (Math.PI / 180), -30 * (Math.PI / 180), -15 * (Math.PI / 180), 1.5, true);

    //  Set the color to red
    c.color = new double[] { 1.0, 0.0, 0.0 };

    // Read the color
    var color_in = c.color;
    Console.WriteLine(string.Join(",", color_in.Select(x => x.ToString())));

    RobotRaconteurNode.s.Sleep(1000);

    // Reset the color
    c.color = new double[] { 0.929, 0.49, 0.192 };

    // Say hello
    c.say("Hello, World From C#!");
}

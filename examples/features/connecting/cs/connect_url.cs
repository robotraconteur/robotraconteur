// connect_url.cs - Connect to a service using a URL

using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;

// Initialize the client node
using (var node_setup = new ClientNodeSetup(args))
{
    // Connect to the Reynard service using a URL
    var c = (experimental.reynard_the_robot.Reynard)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:29200?service=reynard");

    // Say hello
    c.say("Hello from connect_url!");
}

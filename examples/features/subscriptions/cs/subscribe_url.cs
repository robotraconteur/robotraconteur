// subscribe_url.cs - Subscribe to a service using a URL

using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;

// Initialize the client node
using (var node_setup = new ClientNodeSetup(args))
{
    // Subscribe to the service using a URL
    string url = "rr+tcp://localhost:29200?service=reynard";
    var sub = RobotRaconteurNode.s.SubscribeService(url);

    // Get the default client
    var c = (experimental.reynard_the_robot.Reynard)sub.GetDefaultClientWait(10000);

    // Say hello
    c.say("Hello from subscribe_url!");
}

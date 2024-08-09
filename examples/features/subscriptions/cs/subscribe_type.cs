// subscribe_type.cs - Subscribe to a service by type

using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;

// Initialize the client node
using (var node_setup = new ClientNodeSetup(args))
{
    // Subscribe to a service by type
    string[] root_object_type = new string[] { "experimental.reynard_the_robot.Reynard" };
    var sub = RobotRaconteurNode.s.SubscribeServiceByType(root_object_type);

    // Give a few seconds for the service to be connected
    Thread.Sleep(6000);

    // Run a function on all connected services. Could also use GetDefaultClientWait() to
    // get the first connected client and run functions on that client
    // without waiting for all clients to connect
    var connected_clients = sub.GetConnectedClients();
    foreach (var c1 in connected_clients)
    {
        var c = (experimental.reynard_the_robot.Reynard)c1.Value;
        c.say("Hello from subscribe_type!");
    }
}

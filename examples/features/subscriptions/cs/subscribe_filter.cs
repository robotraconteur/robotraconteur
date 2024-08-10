// subscribe_filter.cs - Subscribe to a service by type using a filter

using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;

bool subscription_predicate(ServiceInfo2 service_info)
{
    // This function is called for each discovered service
    // Return true to connect to the service, false to ignore
    // The service_info2 parameter is a ServiceInfo2 object
    // containing information about the discovered service
    return true;
}

// Initialize the client node
using (var node_setup = new ClientNodeSetup(args))
{

    // Populate a subscription filter. Note that all empty fields are considered "wildcards" that match any value
    var f = new ServiceSubscriptionFilter();
    // Only connect to services named "reynard"
    f.ServiceNames = new string[] { "reynard" };
    // Only use rr+tcp and rr+local transport
    f.TransportSchemes = new string[] { "rr+tcp", "rr+local" };
    // The ServiceSubscriptionFilter has powerful attribute matching capabilities
    // The use is intended for advanced use cases
    // f.Attributes = <Advanced attribute matching>
    // f.AttributesMatchOperation = ServiceSubscriptionFilterAttributeGroupOperation.AND;
    // Set a "predicate" function that is called for each discovered service
    f.Predicate = subscription_predicate;
    // Set the maximum number of clients to connect. Default is -1 (no limit)
    f.MaxConnections = 5;

    // Create a ServiceSubscriptionFilterNode for node matching
    var n = new ServiceSubscriptionFilterNode();
    // Only connect to nodes named "experimental.reynard_the_robot"
    n.NodeName = "experimental.reynard_the_robot";
    // Only connect to nodes with NodeID as listed. This will be different for every instance of the node
    // n.NodeID = new NodeID('12345678-1234-1234-1234-123456789abc');
    // Username and credentials can also be set for services that require authentication.
    // This should only be used when NodeID is also specified so the credentials are not sent to the wrong node
    // n.Username = "username"
    // auto credentials = RR::AllocateEmptyRRMap<std::string, RR::RRValue>();
    // credentials->insert({"password", RR::VarValue("my_password", "string")});
    // n.Credentials = credentials;
    f.Nodes = new ServiceSubscriptionFilterNode[] { n };

    // Subscribe to a service by type
    string[] root_object_type = new string[] { "experimental.reynard_the_robot.Reynard" };
    var sub = RobotRaconteurNode.s.SubscribeServiceByType(root_object_type, f);

    // Give a few seconds for the service to be connected
    Thread.Sleep(6000);

    // Run a function on all connected services. Could also use GetDefaultClientWait() to
    // get the first connected client and run functions on that client
    // without waiting for all clients to connect
    var connected_clients = sub.GetConnectedClients();
    foreach (var c1 in connected_clients)
    {
        var c = (experimental.reynard_the_robot.Reynard)c1.Value;
        c.say("Hello from subscribe_filter!");
    }
}

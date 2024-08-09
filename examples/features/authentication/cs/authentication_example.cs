// authentication_example.cs - Example of handling exceptions in C#

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;

using (var node_setup = new ClientNodeSetup(args))
{
    // Create the user credentials
    string username = "testuser1";
    var cred = new Dictionary<string, object>() { { "password", "testpass1" } };

    // Connect to the service
    var c = (experimental.reynard_the_robot.Reynard)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53226?service=authentication_example", username, cred);

    // Use the service
    c.say("Message to secure service");
}

// web_https_client.py - Example of connecting to a standard
// web server using HTTPS and WebSockets

using System;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using System.Linq;
using System.Diagnostics;

// Initialize the client node
using (var node_setup = new ClientNodeSetup(args))
{
    // Connect to the service using rr+wss, the secure WebSocket transport
    var c = (experimental.wstest.testobj)RobotRaconteurNode.s.ConnectService(
        "rr+wss://wstest2.wasontech.com/robotraconteur?service=testobj");

    Debug.Assert(c.add_two_numbers(2, 4) == 6);
}

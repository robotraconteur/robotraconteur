using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using experimental.create3;
using System.Threading;

// Initialize the client node
using (var node_setup = new ClientNodeSetup())
{

    // Connect to the service
    Create c = (Create)RobotRaconteurNode.s.ConnectService("rr+tcp://localhost:22354/?service=create", null, null, null,
                                                           "experimental.create3.Create");

    // Set an event handler for the "Bump" event
    c.bump += Bump;

    // Connect the state wire and add a value changed event handler
    Wire<CreateState>.WireConnection wire = c.create_state.Connect();
    wire.WireValueChanged += wire_changed;

    // Set a function to be used by the callback.  This function will be called
    // when the service calls a callback with the endpoint corresponding to this
    // client
    c.play_callback.Function = play_callback;
    c.claim_play_callback();

    // Drive a bit
    c.drive(0.2, 5.0);
    Thread.Sleep(1000);
    c.drive(0, 0);
    Thread.Sleep(5000);

    // Close the wire
    wire.Close();
}

// Function to handle the "Bump" event
static void Bump()
{
    Console.WriteLine("Bump");
}

// Function to handle when the wire value changes
static void wire_changed(Wire<CreateState>.WireConnection wire_connection, CreateState value, TimeSpec time)
{
    CreateState value2 = wire_connection.InValue;
    Console.WriteLine(value2.time);
}

// Function that is called by the service as a callback.  This returns
// a few notes to play.
static byte[] play_callback(double dist, double angle)
{
    return new byte[] { 69, 16, 60, 16, 69, 16 };
}

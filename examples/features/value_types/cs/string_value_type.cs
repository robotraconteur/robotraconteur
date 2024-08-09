// string_value_type.cs - Example of using string value types

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;

using (var node_setup = new ClientNodeSetup(args))
{

    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Strings
    // property string f_string
    var f_string_read = c.f_string;
    Console.WriteLine(f_string_read);
    // assert is used for the rest of the example property read operations
    // to demonstrate the expected values.
    Debug.Assert(c.f_string == "An example string read from the service");
    c.f_string = "An example string written to the service";
}

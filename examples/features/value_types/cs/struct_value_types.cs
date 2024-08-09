// struct_value_type.cs - Simple example of using struct value types

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using experimental.value_types;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Create and populate a MyStructure
    var s = new MyStructure();
    // field double a
    s.a = 5;
    // field uint32[2] b
    s.b = new uint[] { 10, 20 };
    // field string c
    s.c = "String from structure client";
    // field string{list} d
    s.d = new List<string>( { "string a", "string b" });

    // Set the property using the structure
    // property MyStructure k_struct
    c.k_struct = s;

    // Retrieve the structure from the service
    var u = c.k_struct;

    // Structures can be None
    Debug.Assert(u != null);

    // field double a
    Debug.Assert(u.a == 52);
    // field uint32[2] b
    Debug.Assert(Enumerable.SequenceEqual(u.b, new uint[] { 110, 120 }));
    // field string c
    Debug.Assert(u.c == "String from structure service");
    // field string{list} d
    Debug.Assert(Enumerable.SequenceEqual(u.d, new List<string>( { "string c", "string d", "string e" })));
}

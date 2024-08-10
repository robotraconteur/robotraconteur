// null_value_types.cs - Example of types that can be null

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Null values
    // Null values are "null" in C#
    // struct, containers, and varvalue are nullable.
    // Numbers, arrays, strings, pods, and namedarrays are not nullable.

    // property MyStructure t_struct_null
    c.t_struct_null = null;
    Debug.Assert(c.t_struct_null == null);
}

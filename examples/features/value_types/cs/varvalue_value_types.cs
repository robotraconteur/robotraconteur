// varvalue_value_types.cs - Example of using varvalue wildcard type

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;
using experimental.value_types;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Any valid Robot Raconteur data type can be stored in a varvalue

    // property varvalue p_varvalue_double_array
    // Example of storing a double array in a varvalue
    var a = new double[] { 1.1, 2.2, 3.3 };
    c.p_varvalue_double_array = a;
    var b = (double[])c.p_varvalue_double_array;
    Debug.Assert(Enumerable.SequenceEqual(b, new double[] { 1.4, 2.5, 3.6 }));

    // property varvalue q_varvalue_string
    // Example of storing a string in a varvalue
    c.q_varvalue_string = "varvalue string from client";
    var d = (string)c.q_varvalue_string;
    Debug.Assert(d == "varvalue string from service");

    // property varvalue r_varvalue_struct
    // Example of storing a structure in a varvalue
    var s = new MyStructure() { a = 5, b = new uint[] { 10, 20 }, c = "String from structure client",
                                d = new List<string>() { "string a", "string b" } };

    c.r_varvalue_struct = s;

    var t = (MyStructure)c.r_varvalue_struct;
    Debug.Assert(t.a == 52);
    Debug.Assert(Enumerable.SequenceEqual(t.b, new uint[] { 110, 120 }));
    Debug.Assert(t.c == "String from structure service");
    Debug.Assert(Enumerable.SequenceEqual(t.d, new List<string>() { "string c", "string d", "string e" }));

    // property varvalue{string} s_varvalue_map2
    // Example of storing a map of strings in a varvalue
    // varvalue{string} is used extensively in structures
    // to allow for additional fields to be added to the
    // structure without changing the structure definition
    c.s_varvalue_map2 = new Dictionary<string, object>() { { "key1", new int[] { 2, 3 } }, { "key2", "string 2" } };

    var u = c.s_varvalue_map2;
    Debug.Assert(u.Count == 2);
    Debug.Assert(Enumerable.SequenceEqual((int[])u["key3"], new int[] { 4, 5 }));
    Debug.Assert((string)u["key4"] == "string 4");
}

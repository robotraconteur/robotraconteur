// container_value_types.cs - Example of using container value types (maps and lists)

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

    // property double{int32} l_double_map
    // int32 keyed map of scalar doubles
    c.l_double_map = new Dictionary<int, double>() { { 1, 1.1 }, { 2, 2.2 }, { 5, 3.3 } };

    var b = c.l_double_map;
    Debug.Assert(b.Count == 2);
    Debug.Assert(b[1] == 5.1);
    Debug.Assert(b[5] == 6.2);

    // property double[]{string} l_double_array_map
    // string keyed map of double arrays
    c.l_double_array_map = new Dictionary<string, double[]>() { { "key1", new double[] { 1.1, 1.2, 1.3 } },
                                                                { "key2", new double[] { 2.1, 2.2, 2.3 } } };

    var d = c.l_double_array_map;
    Debug.Assert(d.Count == 2);
    Debug.Assert(d["key3"].SequenceEqual(new double[] { 5.1, 5.2, 5.3 }));
    Debug.Assert(d["key4"].SequenceEqual(new double[] { 6.1, 6.2, 6.3 }));

    // property string{list} m_string_list
    // list of strings
    c.m_string_list = new List<string>() { "string 1", "string 2", "string 3" };

    var a = c.m_string_list;
    Debug.Assert(a.Count == 2);
    Debug.Assert(a[0] == "string 4");
    Debug.Assert(a[1] == "string 5");

    // property string{int32} m_string_map_int32
    // int32 keyed map of strings
    c.m_string_map_int32 =
        new Dictionary<int, string>() { { 12, "string 1" }, { 100, "string 2" }, { 17, "string 3" } };

    var e = c.m_string_map_int32;
    Debug.Assert(e.Count == 2);
    Debug.Assert(e[87] == "string 4");
    Debug.Assert(e[1] == "string 5");

    // property string{string} m_string_map_string
    // string keyed map of strings
    c.m_string_map_string = new Dictionary<string, string>() { { "key1", "string 1" }, { "key2", "string 2" } };

    var f = c.m_string_map_string;
    Debug.Assert(f.Count == 3);
    Debug.Assert(f["key3"] == "string 3");
    Debug.Assert(f["key4"] == "string 4");
    Debug.Assert(f["key5"] == "string 5");

    // property MyVector3{int32} n_vector_map
    // int32 keyed map of MyVector3
    var g = new MyVector3() { x = 1.0, y = 2.0, z = 3.0 };
    c.n_vector_map = new Dictionary<int, MyVector3>() { { 1, g } };

    var g1 = c.n_vector_map;
    Debug.Assert(g1.Count == 1);
    Debug.Assert(g1[1].x == 4.0);
    Debug.Assert(g1[1].y == 5.0);
    Debug.Assert(g1[1].z == 6.0);

    // property MyStructure{list} o_struct_list
    // list of MyStructure
    var h = new MyStructure() { a = 5, b = new uint[] { 10, 20 }, c = "String from structure client",
                                d = new List<string>() { "string a", "string b" } };
    c.o_struct_list = new List<MyStructure>() { h };

    var i = c.o_struct_list;
    Debug.Assert(i.Count == 1);
    Debug.Assert(i[0].a == 52);
    Debug.Assert(Enumerable.SequenceEqual(i[0].b, new uint[] { 110, 120 }));
    Debug.Assert(i[0].c == "String from structure service");
    Debug.Assert(Enumerable.SequenceEqual(i[0].d, new List<string>() { "string c", "string d", "string e" }));
}

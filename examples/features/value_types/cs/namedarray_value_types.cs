// namedarray_value_types.cs - Example of using namedarray value types

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using experimental.value_types;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // property MyVector3 g_vector
    c.g_vector = new MyVector3 { x = 1.0, y = 2.0, z = 3.0 };
    var b = c.g_vector;
    Debug.Assert(b.x == 4.0);
    Debug.Assert(b.y == 5.0);
    Debug.Assert(b.z == 6.0);

    // Now an array of NamedArrays
    // property MyVector3[] g_vector_array
    var f =
        new MyVector3[] { new MyVector3 { x = 1.0, y = 2.0, z = 3.0 }, new MyVector3 { x = 4.0, y = 5.0, z = 6.0 } };
    c.g_vector_array = f;

    var g = c.g_vector_array;
    Debug.Assert(g.Length == 2);
    Debug.Assert(g[0].x == 7.0);
    Debug.Assert(g[0].y == 8.0);
    Debug.Assert(g[0].z == 9.0);
    Debug.Assert(g[1].x == 10.0);
    Debug.Assert(g[1].y == 11.0);
    Debug.Assert(g[1].z == 12.0);

    // Use plain arrays and convert to and from NamedArrays
    var d = new MyVector3();
    var d1 = new ArraySegment<double>(new double[] { 1.0, 2.0, 3.0 });
    d.AssignFromNumericArray(ref d1);
    c.g_vector = d;
    var e = c.g_vector;
    var e1 = new ArraySegment<double>(new double[3]);
    e.GetNumericArray(ref e1);
    Debug.Assert(Enumerable.SequenceEqual(e1.Array, new double[] { 4.0, 5.0, 6.0 }));

    // property MyQuaternion h_quaternion
    var j = new MyQuaternion() { w = 1.0, x = 0.0, y = 0.0, z = 0.0 };
    c.h_quaternion = j;

    var k = c.h_quaternion;
    Debug.Assert(k.w == 0.707);
    Debug.Assert(k.x == 0.0);
    Debug.Assert(k.y == 0.707);
    Debug.Assert(k.z == 0.0);

    // MyPose is a composite NamedArray with fields of other NamedArrays
    // property MyPose l_pose
    var l = new MyPose() { orientation = new MyQuaternion() { w = 1.0, x = 0.0, y = 0.0, z = 0.0 },
                           position = new MyVector3() { x = 1.0, y = 2.0, z = 3.0 } };

    c.i_pose = l;

    var m = c.i_pose;
    Debug.Assert(m.position.x == 4.0);
    Debug.Assert(m.position.y == 5.0);
    Debug.Assert(m.position.z == 6.0);
    Debug.Assert(m.orientation.w == 0.707);
    Debug.Assert(m.orientation.x == 0.0);
    Debug.Assert(m.orientation.y == 0.707);
    Debug.Assert(m.orientation.z == 0.0);

    // Now use plain arrays and convert to and from NamedArrays
    var n = new MyPose();
    var n1 = new ArraySegment<double>(new double[] { 1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0 });
    n.AssignFromNumericArray(ref n1);
    c.i_pose = n;

    var o = c.i_pose;
    Debug.Assert(Enumerable.SequenceEqual(o.GetNumericArray(), new double[] { 0.707, 0.0, 0.707, 0.0, 4.0, 5.0, 6.0 }));
}

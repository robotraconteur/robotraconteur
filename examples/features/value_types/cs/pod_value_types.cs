// pod_value_types.cs - Example of using pod value types

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using experimental.value_types;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Example using Pods

    // property MyPod j_pod
    var a = new MyPod() { a = 0.928,
                          b = 8374,
                          c = new byte[] { 8, 9, 10, 11 },
                          d = new byte[] { 17 },
                          e = new ushort[] { 1, 4, 2, 5, 3, 6 },
                          f = new MyVector3() { x = 10.1, y = 10.2, z = 10.3 } };
    c.j_pod = a;

    var b = c.j_pod;
    Debug.Assert(b.a == 0.791);
    Debug.Assert(b.b == 1077);
    Debug.Assert(Enumerable.SequenceEqual(b.c, new byte[] { 61, 52, 33, 24 }));
    Debug.Assert(Enumerable.SequenceEqual(b.d, new byte[] { 1, 2 }));
    Debug.Assert(Enumerable.SequenceEqual(b.e, new ushort[] { 7, 10, 8, 11, 9, 12 }));
    Debug.Assert(b.f.x == 20.1);
    Debug.Assert(b.f.y == 20.2);
    Debug.Assert(b.f.z == 20.3);

    // Now an array of Pods
    // property MyPod[] j_pod_array
    var d = new MyPod[] { new MyPod() { a = 0.928, b = 8374, c = new byte[] { 8, 9, 10, 11 }, d = new byte[] { 17 },
                                        e = new ushort[] { 1, 4, 2, 5, 3, 6 },
                                        f = new MyVector3() { x = 10.1, y = 10.2, z = 10.3 } },
                          new MyPod() { a = 0.67, b = 123, c = new byte[] { 1, 2, 3, 4 }, d = new byte[] { 5, 6 },
                                        e = new ushort[] { 13, 16, 14, 17, 15, 18 },
                                        f = new MyVector3() { x = 30.1, y = 30.2, z = 30.3 } } };
    c.j_pod_array = d;

    var e = c.j_pod_array;
    Debug.Assert(e[0].a == 0.791);
    Debug.Assert(e[0].b == 1077);
    Debug.Assert(Enumerable.SequenceEqual(e[0].c, new byte[] { 61, 52, 33, 24 }));
    Debug.Assert(Enumerable.SequenceEqual(e[0].d, new byte[] { 1, 2 }));
    Debug.Assert(Enumerable.SequenceEqual(e[0].e, new ushort[] { 7, 10, 8, 11, 9, 12 }));
    Debug.Assert(e[0].f.x == 20.1);
    Debug.Assert(e[0].f.y == 20.2);
    Debug.Assert(e[0].f.z == 20.3);
    Debug.Assert(e[1].a == 0.03);
    Debug.Assert(e[1].b == 693);
    Debug.Assert(Enumerable.SequenceEqual(e[1].c, new byte[] { 5, 6, 7, 8 }));
    Debug.Assert(Enumerable.SequenceEqual(e[1].d, new byte[] { 3 }));
    Debug.Assert(Enumerable.SequenceEqual(e[1].e, new ushort[] { 19, 22, 20, 23, 21, 24 }));
    Debug.Assert(e[1].f.x == 40.1);
    Debug.Assert(e[1].f.y == 40.2);
    Debug.Assert(e[1].f.z == 40.3);
}

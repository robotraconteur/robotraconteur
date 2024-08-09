// numeric_value_types.cs - Example of using numeric value types

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;

using (var node_setup = new ClientNodeSetup(args))
{

    var c = (experimental.value_types.ValueTypesExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53223?service=values_example");

    // Scalar numbers
    // property double a_double
    double a_double_read = c.a_double;
    Console.WriteLine(a_double_read);
    // Debug.Assert is used for the rest of the example property read operations
    // to demonstrate the expected values.
    Debug.Assert(c.a_double == 5.78);
    c.a_double = 49.3;
    // property int32 b_int
    Debug.Assert(c.b_int == 4557);
    c.b_int = (int)359;
    // property uint8 c_byte
    Debug.Assert(c.c_byte == 0x1A);
    c.c_byte = (byte)31;
    // property cdouble d_cdouble
    Debug.Assert(c.d_cdouble == new CDouble(23.7, 5.3));
    c.d_cdouble = new CDouble(1.2, 3.4);
    // property bool e_bool
    Debug.Assert(c.e_bool == true);
    c.e_bool = false;
    // property int32 meaning_of_life [readonly]
    Debug.Assert(c.meaning_of_life == 42);

    // Numeric Arrays
    // property double[] a_double_array
    Debug.Assert(Enumerable.SequenceEqual(c.a_double_array, new double[] { 0.016, 0.226 }));
    c.a_double_array = new double[] { 0.582, 0.288, 0.09, 0.213, 0.98 };
    // property double[3] a_double_array_fixed
    Debug.Assert(Enumerable.SequenceEqual(c.a_double_array_fixed, new double[] { 0.13, 0.27, 0.15 }));
    c.a_double_array_fixed = new double[] { 0.21, 0.12, 0.39 };
    // % property double[6-] a_double_array_maxlen
    Debug.Assert(Enumerable.SequenceEqual(c.a_double_array_maxlen, new double[] { 0.7, 0.16, 0.16, 0.05, 0.61, 0.9 }));
    c.a_double_array_maxlen = new double[] { 0.035, 0.4 };
    // property double[3,2] a_double_marray_fixed
    var a_double_marray_fixed_read = c.a_double_marray_fixed;
    Debug.Assert(Enumerable.SequenceEqual(a_double_marray_fixed_read.Dims, new uint[] { 3, 2 }));
    Debug.Assert(Enumerable.SequenceEqual((double[])a_double_marray_fixed_read.Array_,
                                          new double[] { 0.29, 0.41, 0.4, 0.66, 0.6, 0.2 }));
    c.a_double_marray_fixed = new MultiDimArray(new uint[] { 3, 2 }, new double[] { 0.3, 0.4, 0.5, 0.6, 0.6, 0.2 });
    // property double[*] a_double_marray
    var a_double_marray_read = c.a_double_marray;
    Debug.Assert(Enumerable.SequenceEqual(a_double_marray_read.Dims, new uint[] { 2, 2 }));
    Debug.Assert(
        Enumerable.SequenceEqual((double[])a_double_marray_read.Array_, new double[] { 0.72, 0.05, 0.4, 0.07 }));
    c.a_double_marray = new MultiDimArray(new uint[] { 2, 1 }, new double[] { 0.3, 0.01 });
    // property uint8[] c_byte_array
    Debug.Assert(Enumerable.SequenceEqual(c.c_byte_array, new byte[] { 0x1A, 0x2B }));
    c.c_byte_array = new byte[] { 0x3C, 0x4D, 0x5E, 0x6F, 0x70 };
}

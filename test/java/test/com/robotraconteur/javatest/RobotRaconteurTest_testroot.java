package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

import java.util.*;

// cSpell: ignore lstruct, fireev, addev, removeev

public class RobotRaconteurTest_testroot implements testroot
{
    public RobotRaconteurTest_testroot(TcpTransport tcptransport)
    {
        this.tcptransport = tcptransport;
    }

    public final double get_d1()
    {
        return 12.345;
    }
    public final void set_d1(double value)
    {
        RRAssert.areEqual(value, 3.456);
    }

    public final double[] get_d2()
    {
        return TestVectors.getDoubles("ts1.d2.to_client");
    }
    public final void set_d2(double[] value)
    {
        double[] testval = TestVectors.getDoubles("ts1.d2.to_service");
        ca(testval, value);
    }

    public final void ca(double[] v1, double[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(float[] v1, float[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(byte[] v1, byte[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(short[] v1, short[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(int[] v1, int[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(long[] v1, long[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final double[] get_d3()
    {
        return new double[] {2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11,
                             9.818531e-21, 6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00,
                             3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21};
    }
    public final void set_d3(double[] value)
    {
        double[] testval =
            new double[] {9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01,
                          1.969554e+12, 9.451979e-05, 5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13,
                          2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14};
        ca(testval, value);
    }

    public final double[] get_d4()
    {
        return new double[] {2.864760e-08, 3.900663e+13, 9.105789e+11, 2.943743e-15, -2.823159e-16, -3.481261e+19};
    }
    public final void set_d4(double[] value)
    {
        double[] testval = new double[] {-4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11,
                                         3.511191e-08,  4.759250e-07,  2.455883e+09,  4.182578e+11,  4.732337e-14,
                                         -2.967313e+02, -4.139188e+14, 6.287269e+03};
        ca(value, testval);
    }

    public final MultiDimArray get_d5()
    {
        double[] r = TestVectors.getDoubles("ts1.d5.to_client");

        int[] dims = new int[] {5, 6, 5};
        return new MultiDimArray(dims, r);
    }
    public final void set_d5(MultiDimArray value)
    {
        double[] r = TestVectors.getDoubles("ts1.d5.to_service");

        int[] dims = new int[] {8, 6, 2};

        ca(dims, value.dims);
        ca(r, (double[])value.array);
    }

    public final float get_s1()
    {
        return 7.8573f;
    }
    public final void set_s1(float value)
    {
        RRAssert.areEqual(value, 3847.9283f);
    }

    public final float[] get_s2()
    {
        return TestVectors.getFloats("ts1.s2.to_client");
    }
    public final void set_s2(float[] value)
    {
        float[] testval = TestVectors.getFloats("ts1.s2.to_service");
        ca(value, testval);
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public sbyte get_i8_1()
    public final byte get_i8_1()
    {
        return -66;
    }
    public final void set_i8_1(byte value)
    {
        RRAssert.areEqual(value, 45);
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public sbyte[] get_i8_2()
    public final byte[] get_i8_2()
    {
        return TestVectors.getBytes("ts1.i8_2.to_client");
    }
    public final void set_i8_2(byte[] value)
    {
        byte[] testval = TestVectors.getBytes("ts1.i8_2.to_service");
        ca(value, testval);
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public byte get_u8_1()
    public final UnsignedByte get_u8_1()
    {
        return new UnsignedByte((byte)-34);
    }
    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public void set_u8_1(byte value)
    public final void set_u8_1(UnsignedByte value)
    {
        RRAssert.areEqual(value.value, -24);
    }

    public final UnsignedBytes get_u8_2()
    {
        return new UnsignedBytes(TestVectors.getUInt8s("ts1.u8_2.to_client"));
    }

    public final void set_u8_2(UnsignedBytes value)
    {
        byte[] testval = TestVectors.getUInt8s("ts1.u8_2.to_service");
        ca(value.value, testval);
    }

    public final MultiDimArray get_u8_3()
    {
        byte[] r = TestVectors.getUInt8s("ts1.u8_3.to_client");
        int[] dims = new int[] {10, 20};
        return new MultiDimArray(dims, new UnsignedBytes(r));
    }
    public final void set_u8_3(MultiDimArray value)
    {
        byte[] testval = TestVectors.getUInt8s("ts1.u8_3.to_service");
        int[] testdims = new int[] {30, 10};
        ca(value.dims, testdims);
        ca(((UnsignedBytes)value.array).value, testval);
    }
    public final short get_i16_1()
    {
        return -13428;
    }
    public final void set_i16_1(short value)
    {
        RRAssert.areEqual(value, 2387);
    }
    public final short[] get_i16_2()
    {
        return TestVectors.getShorts("ts1.i16_2.to_client");
    }
    public final void set_i16_2(short[] value)
    {
        short[] testval = TestVectors.getShorts("ts1.i16_2.to_service");
        ca(value, testval);
    }

    public final UnsignedShort get_u16_1()
    {
        return new UnsignedShort((short)-4555);
    }
    public final void set_u16_1(UnsignedShort value)
    {
        RRAssert.areEqual(value.value, -10804);
    }

    public final UnsignedShorts get_u16_2()
    {
        return new UnsignedShorts(TestVectors.getUInt16s("ts1.u16_2.to_client"));
    }

    public final void set_u16_2(UnsignedShorts value)
    {
        short[] testval = TestVectors.getUInt16s("ts1.u16_2.to_service");
        ca(value.value, testval);
    }

    public final int get_i32_1()
    {
        return 898734;
    }
    public final void set_i32_1(int value)
    {
        RRAssert.areEqual(value, -9837284);
    }

    public final int[] get_i32_2()
    {
        return TestVectors.getInts("ts1.i32_2.to_client");
    }
    public final void set_i32_2(int[] value)
    {
        int[] testval = TestVectors.getInts("ts1.i32_2.to_service");
        ca(value, testval);
    }

    public int[] get_i32_huge()
    {

        int[] o = new int[2621440];
        for (int i = 0; i < 2621440; i++)
        {
            o[i] = i;
        }
        return o;
    }

    public void set_i32_huge(int[] value)
    {
        RRAssert.areEqual(value.length, 2621440);
        for (int i = 0; i < 2621440; i++)
        {
            RRAssert.areEqual(value[i], -i);
        }
    }

    public final UnsignedInt get_u32_1()
    {
        return new UnsignedInt(547919123);
    }

    public final void set_u32_1(UnsignedInt value)
    {
        RRAssert.areEqual(value.value, 1550099195);
    }

    public final UnsignedInts get_u32_2()
    {
        return new UnsignedInts(TestVectors.getUInt32s("ts1.u32_2.to_client"));
    }

    public final void set_u32_2(UnsignedInts value)
    {

        int[] testval = TestVectors.getUInt32s("ts1.u32_2.to_service");
        ca(testval, value.value);
    }
    public final long get_i64_1()
    {
        return -1357833931563696072L;
    }
    public final void set_i64_1(long value)
    {
        RRAssert.areEqual(value, 8621740821050813024L);
    }

    public final long[] get_i64_2()
    {
        return TestVectors.getLongs("ts1.i64_2.to_client");
    }
    public final void set_i64_2(long[] value)
    {
        long[] testval = TestVectors.getLongs("ts1.i64_2.to_service");
    }

    public final UnsignedLong get_u64_1()
    {
        return new UnsignedLong(-5056882102845907238L);
    }

    public final void set_u64_1(UnsignedLong value)
    {
        RRAssert.areEqual(value.value, 1465640522145789825L);
    }

    public final UnsignedLongs get_u64_2()
    {
        return new UnsignedLongs(TestVectors.getUInt64s("ts1.u64_2.to_client"));
    }
    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public void set_u64_2(ulong[] value)
    public final void set_u64_2(UnsignedLongs value)
    {
        long[] testval = TestVectors.getUInt64s("ts1.u64_2.to_service");
        ca(value.value, testval);
    }
    public final String get_str1()
    {
        return "Hello Client!";
    }
    public final void set_str1(String value)
    {
        RRAssert.areEqual(value, "Hello Server!");
    }

    public final teststruct1 get_struct1()
    {
        teststruct1 s = new teststruct1();
        s.dat1 = TestVectors.getDoubles("ts1.struct1.dat1.to_client");
        s.str2 = "Hello world!";

        java.util.HashMap<Integer, String> vec3 = new java.util.HashMap<Integer, String>();
        vec3.put(1, "Hello Client!");
        vec3.put(2, "Hello Client, again");
        vec3.put(4372, "This is yet another test string");
        s.vec3 = vec3;

        java.util.HashMap<String, String> dict4 = new java.util.HashMap<String, String>();
        dict4.put("teststring1", "Hello Client!");
        dict4.put("teststring2", "Hello Client, again");
        dict4.put("anotherstr", "This is yet another test string");
        s.dict4 = dict4;

        java.util.List<String> list5 = new java.util.ArrayList<String>();
        list5.add("Hello Client!");
        list5.add("Hello Client, again");
        list5.add("This is yet another test string");
        s.list5 = list5;

        teststruct2 s2 = new teststruct2();
        s2.mydat = TestVectors.getDoubles("ts1.struct1.struct1_mydat.to_client");
        s.struct1 = s2;

        java.util.HashMap<String, teststruct2> d2 = new java.util.HashMap<String, teststruct2>();
        teststruct2 d2_1 = new teststruct2();
        d2_1.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client");
        d2.put("test1", d2_1);
        teststruct2 d2_2 = new teststruct2();
        d2_2.mydat =
            new double[] {4.856615e+15,  5.981566e-22, 1.433616e+14,  1.747102e-09, 2.850376e+06,  -3.748685e-08,
                          -4.969544e-21, 2.530419e-01, 4.393913e-09,  3.837331e+04, -4.315065e-04, -1.073834e-17,
                          1.244057e-15,  3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13,  -1.937982e+02};
        d2.put("anothertest", d2_2);

        s.dstruct2 = d2;

        java.util.List<teststruct2> l3 = new java.util.ArrayList<teststruct2>();
        teststruct2 l3_1 = new teststruct2();
        l3_1.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client");
        l3.add(l3_1);
        teststruct2 l3_2 = new teststruct2();
        l3_2.mydat =
            new double[] {4.856615e+15,  5.981566e-22, 1.433616e+14,  1.747102e-09, 2.850376e+06,  -3.748685e-08,
                          -4.969544e-21, 2.530419e-01, 4.393913e-09,  3.837331e+04, -4.315065e-04, -1.073834e-17,
                          1.244057e-15,  3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13,  -1.937982e+02};
        l3.add(l3_2);
        s.lstruct3 = l3;

        double[] md = TestVectors.getDoubles("ts1.struct1.multidimarray.to_client");
        s.multidimarray = new MultiDimArray(new int[] {10, 10}, md);
        s.var3 = "This is a vartype string";
        return s;
    }
    public final void set_struct1(teststruct1 value)
    {
        ca(value.dat1, TestVectors.getDoubles("ts1.struct1.dat1.to_service"));
        RRAssert.areEqual(value.str2, "Hello world 2!");

        RRAssert.areEqual(value.vec3.size(), 4);
        RRAssert.areEqual(value.vec3.get(10), "Hello Server!");
        RRAssert.areEqual(value.vec3.get(11), "Hello Server, again");
        RRAssert.areEqual(value.vec3.get(46372), "Test string!");
        RRAssert.areEqual(value.vec3.get(46373), "Test string again");

        RRAssert.areEqual(value.dict4.size(), 2);
        RRAssert.areEqual(value.dict4.get("cteststring1"), "Hello Server!");
        RRAssert.areEqual(value.dict4.get("cteststring2"), "Hello Server, again");

        RRAssert.areEqual(value.list5.size(), 2);
        RRAssert.areEqual(value.list5.get(0), "Hello Server!");
        RRAssert.areEqual(value.list5.get(1), "Hello Server, again");

        ca(value.struct1.mydat, TestVectors.getDoubles("ts1.struct1.struct1_mydat.to_service"));
        ca(value.dstruct2.get("ctest1").mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_service"));
        ca(value.dstruct2.get("anothertest").mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_2.to_service"));

        ca(value.lstruct3.get(0).mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_service"));
        ca(value.lstruct3.get(1).mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_2.to_service"));

        ca(value.multidimarray.dims, new int[] {10, 10});
        ca((double[])value.multidimarray.array, TestVectors.getDoubles("ts1.struct1.multidimarray.to_service"));

        ca((double[])value.var3, new double[] {6.404176e-12, 9.258110e-03, 8.657620e-03, -2.064381e+00, 5.182360e-16,
                                               4.167658e-16, -4.533051e-19, 5.357520e+18, -4.990383e-13, 2.286982e+08,
                                               -4.727256e-18, 1.465299e-17, 3.000340e-10, -2.304453e-04});
    }

    public final teststruct2 get_struct2()
    {
        teststruct2 a = new teststruct2();
        a.mydat = TestVectors.getDoubles("ts1.struct2.to_client");
        return a;
    }

    public final void set_struct2(teststruct2 value)
    {
        ca(value.mydat, TestVectors.getDoubles("ts1.struct2.to_service"));
    }

    public final java.util.Map<Integer, double[]> get_is_d1()
    {
        java.util.HashMap<Integer, double[]> o = new java.util.HashMap<Integer, double[]>();
        o.put(9285, new double[] {1.643392e-01});
        o.put(74822, new double[] {1.537133e+09});
        o.put(4, new double[] {1.369505e-03});
        return o;
    }

    public final void set_is_d1(java.util.Map<Integer, double[]> value)
    {
        RRAssert.areEqual(value.size(), 3);
        RRAssert.areEqual(value.get(928)[0], 4.074501e-07);
        RRAssert.areEqual(value.get(394820)[0], -4.535303e+05);
        RRAssert.areEqual(value.get(623)[0], -2.956241e-20);
    }
    public final java.util.Map<String, double[]> get_is_d2()
    {
        java.util.HashMap<String, double[]> o = new java.util.HashMap<String, double[]>();
        o.put("testval1", new double[] {-1.079664e+16});
        o.put("testval2", new double[] {2.224846e+00});
        return o;
    }
    public final void set_is_d2(java.util.Map<String, double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        RRAssert.areEqual(value.get("testval3")[0], 5.242474e+10);
        RRAssert.areEqual(value.get("testval4")[0], 2.208636e+08);
    }

    public final java.util.Map<Integer, double[]> get_is_d3()
    {
        java.util.HashMap<Integer, double[]> o = new java.util.HashMap<Integer, double[]>();
        o.put(12, new double[] {8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                                2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                                -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                                -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01});
        o.put(832, new double[] {4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                                 4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                                 2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                                 1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02});
        return o;
    }
    public final void set_is_d3(java.util.Map<Integer, double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get(47), new double[] {4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                        -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                        4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                        -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15});
        ca(value.get(324), new double[] {3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                         7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                         5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                         -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05});
    }

    public final java.util.Map<String, double[]> get_is_d4()
    {
        java.util.HashMap<String, double[]> o = new java.util.HashMap<String, double[]>();
        o.put("testval1", new double[] {1.113851e-04,  3.830104e+07, 4.571169e-21,  -4.064180e-05, 2.889736e+01,
                                        -1.790060e-06, 4.608538e+00, 4.687713e-04,  1.387717e-08,  3.914187e-18,
                                        -5.618118e-06, 1.530811e+05, -5.848922e-11, -3.397558e-20, -6.597368e-08,
                                        -3.779049e-06, 2.406033e-19, 2.507939e-10,  3.246113e-20,  1.341205e+16});
        o.put("testval2", new double[] {-3.088190e-13, -4.033334e-20, 4.150103e-21,  -6.610855e+17, 3.688824e-13,
                                        -3.208025e+13, -5.034888e-11, -4.098363e-06, -1.272830e-03, 2.748392e-03,
                                        -2.644272e-06, -4.810065e-18, 4.629861e-19,  -5.444015e-03, 4.046008e+17,
                                        -3.548079e+12, -3.455290e+16, -3.668946e-12, -3.522178e-01, -1.537583e+14});
        return o;
    }
    public final void set_is_d4(java.util.Map<String, double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get("testval3"),
           new double[] {1.771838e+06, 3.037284e-01,  -1.739742e-02, 1.399508e-20,  3.605232e-21,
                         3.517522e+14, 4.887514e+14,  3.505442e-03,  -3.968972e+18, 1.422037e-20,
                         2.596937e-21, 4.852833e-11,  6.852955e-17,  4.765526e-12,  -3.445954e+16,
                         2.322531e-14, -1.755122e-12, 3.941875e+00,  8.877046e-13,  2.818923e-02});
        ca(value.get("testval4"),
           new double[] {4.146439e+16,  2.923439e-07,  3.549608e+16,  -1.664891e-01, -4.192309e-15,
                         3.857317e+05,  -1.101076e+00, 1.213105e+19,  3.237584e-14,  -2.421219e-06,
                         -4.603196e-05, -3.719535e-10, 1.124961e+06,  2.032849e+10,  4.639704e-22,
                         3.946835e+01,  -9.267263e+01, -4.456188e+11, 3.470487e+08,  7.918764e+10});
    }

    public final java.util.Map<Integer, MultiDimArray> get_is_d5()
    {
        MultiDimArray a = new MultiDimArray(new int[] {10, 10}, TestVectors.getDoubles("ts1.is_d5.to_client"));
        java.util.HashMap<Integer, MultiDimArray> o = new java.util.HashMap<Integer, MultiDimArray>();
        o.put(564, a);
        return o;
    }
    public final void set_is_d5(java.util.Map<Integer, MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get(328);
        ca(a.dims, new int[] {10, 20});
        ca((double[])a.array, TestVectors.getDoubles("ts1.is_d5.to_service"));
    }

    public final java.util.Map<String, MultiDimArray> get_is_d6()
    {
        MultiDimArray a = new MultiDimArray(new int[] {5, 10}, TestVectors.getDoubles("ts1.is_d6.to_client"));
        java.util.HashMap<String, MultiDimArray> o = new java.util.HashMap<String, MultiDimArray>();
        o.put("testval1", a);
        return o;
    }
    public final void set_is_d6(java.util.Map<String, MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get("testval2");
        ca(a.dims, new int[] {8, 10});
        ca((double[])a.array, TestVectors.getDoubles("ts1.is_d6.to_service"));
    }

    public final java.util.Map<Integer, String> get_is_str1()
    {
        java.util.HashMap<Integer, String> o = new java.util.HashMap<Integer, String>();
        o.put(23, "Hello server");
        return o;
    }

    public final void set_is_str1(java.util.Map<Integer, String> value)
    {
        RRAssert.areEqual(value.size(), 1);
        RRAssert.areEqual(value.get(24), "Hello client");
    }

    public final java.util.Map<String, String> get_is_str2()
    {
        java.util.HashMap<String, String> o = new java.util.HashMap<String, String>();
        o.put("testval1", "Hello server");
        return o;
    }

    public final void set_is_str2(java.util.Map<String, String> value)
    {
        RRAssert.areEqual(value.size(), 1);
        RRAssert.areEqual(value.get("testval2"), "Hello client");
    }

    public final java.util.Map<Integer, teststruct2> get_is_struct1()
    {
        java.util.HashMap<Integer, teststruct2> o = new java.util.HashMap<Integer, teststruct2>();
        teststruct2 s = new teststruct2();
        s.mydat = new double[] {-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                                -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                                -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                                -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21};
        o.put(748, s);
        return o;
    }
    public final void set_is_struct1(java.util.Map<Integer, teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get(372);
        ca(s.mydat, new double[] {-2.101948e-07, -2.594836e-08, 2.515710e+01,  -3.834127e-14, -3.088095e+06,
                                  -3.256612e-02, -1.855481e-19, 3.801916e+07,  2.145894e+09,  4.487676e+12,
                                  1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00, 1.786029e-20,
                                  -4.991515e+08, 4.006107e-10,  -4.947871e-11, -2.737020e-08, 4.123759e-20});
    }

    public final java.util.Map<String, teststruct2> get_is_struct2()
    {
        java.util.HashMap<String, teststruct2> o = new java.util.HashMap<String, teststruct2>();
        teststruct2 s = new teststruct2();
        s.mydat = new double[] {-4.489570e+13, 9.574895e-05,  4.081711e+06,  5.612839e-18,  -1.078604e+05,
                                3.658139e+08,  -4.748975e+05, -2.606481e+01, 3.016739e+15,  3.174709e+19,
                                -4.572549e+17, 1.980389e-04,  -3.551911e-10, 3.598401e-07,  2.659416e-12,
                                -3.606157e+06, 2.059674e+17,  -9.362336e-20, -3.299256e+17, -2.245745e+16};
        o.put("testval1", s);
        return o;
    }
    public final void set_is_struct2(java.util.Map<String, teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get("testval2");
        ca(s.mydat, new double[] {6.931327e-21,  4.527137e-02,  1.260822e-18,  3.592805e-12,  1.088317e-05,
                                  3.305865e+03,  -9.798828e-20, 1.497504e+18,  -3.653592e+01, 1.473952e+10,
                                  -1.003612e-20, 1.302159e+18,  -8.544326e+05, 1.038521e+16,  -2.845746e-18,
                                  -3.899909e-04, 4.785560e-02,  -7.203365e-12, -1.500022e-14, -1.892753e-17});
    }

    public final ostruct2 get_struct3()
    {
        ostruct2 o = new ostruct2();
        o.a1 = new double[] {-8.483090e-19, -4.401548e-08, 3.908118e+00,  2.063513e-18,  4.237047e+18,
                             -1.124681e-16, 3.924541e-01,  -2.184335e-10, -1.978950e+11, 1.586365e+18,
                             1.712393e+00,  -6.314723e+00, 1.196777e-16,  -2.748704e-08, -1.289967e+02,
                             -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06,  1.315398e+17};
        return o;
    }
    public final void set_struct3(ostruct2 value)
    {
        ca(value.a1, new double[] {-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03,
                                   8.256717e-12,  -4.772119e-11, -1.061407e-13, 2.759750e+02,  -1.212549e+10,
                                   7.012690e+15,  3.953354e+04,  -2.617985e-07, 1.104408e-21,  -3.889366e+00,
                                   4.549493e+16,  -1.376791e+15, -3.445205e-21, 2.137830e-14,  4.620179e+18});
    }

    private Object m_var1 = null;

    public final Object get_var1()
    {
        return m_var1;
    }
    public final void set_var1(Object value)
    {
        m_var1 = value;
    }

    private java.util.Map<Integer, Object> m_var2 = null;

    public final java.util.Map<Integer, Object> get_var2()
    {
        return m_var2;
    }
    public final void set_var2(java.util.Map<Integer, Object> value)
    {
        m_var2 = value;
    }

    public final Object get_var_num()
    {
        return new int[] {-1680284833, -54562307, 732107275,  1470526962, -1389452949,
                          256801409,   261288152, 1728150828, 1322531658, -1640628174,
                          1036878614,  511108054, 2057847386, 288780916,  996595759};
    }
    public final void set_var_num(Object value)
    {
        ca((int[])value, new int[] {-1046369769, 1950632347, 1140727074, -1277424443, 163999900, 970815027, 545593183,
                                    514305170, 1896372264, 1385916382});
    }

    public final Object get_var_str()
    {
        return "Hello Client!";
    }
    public final void set_var_str(Object value)
    {
        RRAssert.areEqual(((String)value), "Hello Server!");
    }

    public final Object get_var_struct()
    {
        teststruct2 s = new teststruct2();
        s.mydat = new double[] {-9.052731e+13, 4.151705e-17,  -4.004463e+19, -2.838274e+03, 9.983314e+12,
                                2.764122e+10,  -1.131486e+03, 2.418899e+12,  1.323675e-05,  -4.602174e+13,
                                2.717530e+01,  1.193887e-10,  -4.137578e+16, -1.246990e-19, 4.244315e-18,
                                -2.833005e-08, 1.956266e-04,  4.130129e-21,  1.641708e-11,  -4.488158e-19};
        return s;
    }
    public final void set_var_struct(Object value)
    {
        ca(((teststruct2)value).mydat,
           new double[] {-4.945426e-20, 1.763386e+13,  3.431578e-04,  4.411409e+17,  -2.690201e+03,
                         3.025939e-10,  -3.659846e+11, -4.780435e-10, -3.246816e+14, -1.815578e+04,
                         2.236455e+10,  -4.639041e+14, 1.767930e+10,  -1.636094e+05, -4.392462e-01,
                         2.225260e+04,  -5.250245e+18, 8.755282e-12,  2.005819e-10,  2.702210e+04});
    }

    public final Object get_var_vector()
    {
        java.util.HashMap<Integer, Object> d = new java.util.HashMap<Integer, Object>();
        d.put(10, "Hello Client!");
        return d;
    }
    public final void set_var_vector(Object value)
    {
        java.util.HashMap<Integer, Object> d = (java.util.HashMap<Integer, Object>)value;
        RRAssert.areEqual(d.size(), 1);
        RRAssert.areEqual(((String)d.get(11)), "Hello Server!");
    }

    public final Object get_var_dictionary()
    {
        java.util.HashMap<String, Object> d = new java.util.HashMap<String, Object>();
        d.put("test1", "Hello Client!");
        return d;
    }
    public final void set_var_dictionary(Object value)
    {
        java.util.HashMap<String, Object> d = (java.util.HashMap<String, Object>)value;
        RRAssert.areEqual(d.size(), 1);
        RRAssert.areEqual(((String)d.get("test2")), "Hello Server!");
    }

    public final Object get_var_list()
    {
        java.util.List<Object> d = new java.util.ArrayList<Object>();
        d.add("Hello Client!");
        return d;
    }
    public final void set_var_list(Object value)
    {
        java.util.List<Object> d = (java.util.List<Object>)value;
        RRAssert.areEqual(d.size(), 1);
        RRAssert.areEqual(((String)d.get(0)), "Hello Server!");
    }

    public final Object get_var_multidimarray()
    {
        double[] d = new double[] {-4.915597e-01, 3.892823e+00,  2.622325e+08,  -7.150935e+04, 9.418756e+00,
                                   3.633879e+18,  3.522383e-03,  -4.989811e+05, 2.027383e-03,  -3.153241e+12,
                                   -6.948245e-21, -3.198577e+14, 6.172905e+09,  3.849430e+15,  8.600383e+13,
                                   4.079437e-17,  3.194775e+06,  4.222550e-18,  1.758122e+17,  -1.018308e+03};
        return new MultiDimArray(new int[] {5, 4}, d);
    }
    public final void set_var_multidimarray(Object value)
    {
        MultiDimArray a = (MultiDimArray)value;
        ca(a.dims, new int[] {5, 4});
        double[] d = new double[] {3.792953e+00,  2.968121e-17,  -3.976413e-15, 4.392986e+19,  2.197463e+10,
                                   -2.627743e-14, -2.184665e+17, 1.972257e-17,  9.929684e-03,  -3.096821e+17,
                                   3.598051e+11,  -6.266015e-18, 1.811985e-11,  2.815232e-07,  7.469467e-06,
                                   6.141798e+13,  3.105763e+09,  -1.697809e-10, -4.141707e-17, 4.391634e+13};
        ca((double[])a.array, d);
    }

    public final double get_errtest()
    {
        throw new RuntimeException("This is a test");
    }
    public final void set_errtest(double value)
    {
        throw new RuntimeException("This is a test");
    }

    public final teststruct1 get_nulltest()
    {
        return null;
    }
    public final void set_nulltest(teststruct1 value)
    {
        RRAssert.areEqual(value, null);
    }

    class func1_run implements Action1<TimerEvent>
    {
        public void action(TimerEvent evt)
        {
            fireev1();
        }
    }

    public final void func1()
    {

        com.robotraconteur.Timer t = RobotRaconteurNode.s().createTimer(1000, new func1_run(), true);
        t.start();
    }

    class func2_run implements Action1<TimerEvent>
    {
        public double d1;
        public double d2;
        public void action(TimerEvent evt)
        {
            teststruct2 s = new teststruct2();
            s.mydat = new double[] {d2};
            fireev2(d1, s);
        }
    }

    public final void func2(double d1, double d2)
    {
        func2_run run = new func2_run();
        run.d1 = d1;
        run.d2 = d2;
        com.robotraconteur.Timer t = RobotRaconteurNode.s().createTimer(1000, run, true);
        t.start();
    }

    TcpTransport tcptransport;

    public final double func3(double d1, double d2)
    {

        try
        {
            /*AuthenticatedUser user=ServerEndpoint.getCurrentAuthenticatedUser();
            System.out.println("User: " + user.getUsername());
            System.out.println("Privilege count: " + user.getPrivileges().length);
            System.out.println("Login: " + user.getLoginTime().toString());
            System.out.println("Access: " + user.getLastAccessTime().toString());*/
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
            System.out.println("No user");
        }

        if (tcptransport.isTransportConnectionSecure(ServerEndpoint.getCurrentEndpoint()))
        {

            if (tcptransport.isSecurePeerIdentityVerified(ServerEndpoint.getCurrentEndpoint()))
            {
                System.out.println("Peer identity is verified: " +
                                   tcptransport.getSecurePeerIdentity(ServerEndpoint.getCurrentEndpoint()));
            }
            else
            {
                System.out.println("Peer identity is not verified");
            }
        }

        return d1 + d2;
    }

    public final int meaning_of_life()
    {
        return 42;
    }

    public final void func_errtest()
    {
        throw new RuntimeException("This is a test");
    }

    Vector<Action> ev1 = new Vector<Action>();

    public void addev1Listener(Action listener)
    {
        ev1.add(listener);
    }
    public void removeev1Listener(Action listener)
    {
        ev1.remove(listener);
    }

    void fireev1()
    {
        for (Action a : ev1)
        {
            try
            {
                a.action();
            }
            catch (Exception e)
            {}
        }
    }

    Vector<Action2<Double, teststruct2>> ev2 = new Vector<Action2<Double, teststruct2>>();
    public void addev2Listener(Action2<Double, teststruct2> listener)
    {
        ev2.add(listener);
    }
    public void removeev2Listener(Action2<Double, teststruct2> listener)
    {
        ev2.remove(listener);
    }

    void fireev2(double d1, teststruct2 s)
    {
        for (Action2<Double, teststruct2> a : ev2)
        {
            try
            {
                a.action(d1, s);
            }
            catch (Exception e)
            {}
        }
    }

    private sub1_impl o1 = new sub1_impl();

    public final sub1 get_o1()
    {
        return o1;
    }

    private java.util.HashMap<Integer, sub1_impl> o2 = new java.util.HashMap<Integer, sub1_impl>();

    public final sub1 get_o2(int ind)
    {
        synchronized (o2)
        {
            if (o2.containsKey(ind))
            {
                return o2.get(ind);
            }

            sub1_impl o = new sub1_impl();
            o.set_i_ind(ind);
            o2.put(ind, o);
            return o;
        }
    }

    private java.util.HashMap<Integer, sub1_impl> o3 = new java.util.HashMap<Integer, sub1_impl>();
    public final sub1 get_o3(int ind)
    {
        synchronized (o3)
        {
            if (o3.containsKey(ind))
            {
                return o3.get(ind);
            }

            sub1_impl o = new sub1_impl();
            o.set_i_ind(ind);
            o3.put(ind, o);
            return o;
        }
    }

    private java.util.HashMap<String, sub1_impl> o4 = new java.util.HashMap<String, sub1_impl>();
    public final sub1 get_o4(String ind)
    {
        synchronized (o4)
        {
            if (o4.containsKey(ind))
            {
                return o4.get(ind);
            }

            sub1_impl o = new sub1_impl();
            o.set_s_ind(ind);
            o4.put(ind, o);
            return o;
        }
    }

    private TestService2SubObj o5 = new TestService2SubObj();
    public final subobj get_o5()
    {
        return o5;
    }

    private Object o6 = null;
    public final Object get_o6()
    {
        return o6;
    }

    public final void o6_op(int op)
    {
        try
        {

            ServerContext.getCurrentServerContext().releaseServicePath(ServerContext.getCurrentServicePath() + ".o6");
        }
        catch (Exception e)
        {}

        switch (op)
        {
        case 0:
            o6 = new sub1_impl();
            break;
        case 1:
            o6 = new sub2_impl();
            break;
        case 2:
            o6 = new TestService2SubObj();
            break;

        default:
            throw new UnsupportedOperationException();
        }
    }

    private Pipe<double[]> m_p1;

    public final Pipe<double[]> get_p1()
    {
        return m_p1;
    }
    public final void set_p1(Pipe<double[]> value)
    {
        m_p1 = (Pipe<double[]>)value;
        m_p1.setPipeConnectCallback(new p1_connect_callback());
    }

    private boolean packet_sent = false;
    private boolean ack_recv = false;
    private long packetnum = 0;

    class p1_connect_callback implements Action1<Pipe<double[]>.PipeEndpoint>
    {
        public void action(Pipe<double[]>.PipeEndpoint p)
        {

            p.addPacketReceivedListener(new p1_packet_received());
            p.addPacketAckReceivedListener(new p1_packet_ack_received());
            p.setRequestPacketAck(true);
        }
    }

    private Object p1_lock = new Object();

    class p1_packet_received implements Action1<Pipe<double[]>.PipeEndpoint>, Action
    {

        public void action()
        {
            try
            {
                Thread.sleep(500);
            }
            catch (Exception e)
            {}
            synchronized (p1_lock)
            {
                while (p.available() > 0)
                {
                    try
                    {
                        double[] dat = p.receivePacket();
                        long pnum = p.sendPacket(dat);
                        if (!packet_sent)
                        {
                            packetnum = pnum;
                        }
                        packet_sent = true;
                    }
                    catch (Exception ee)
                    {}
                }
            }
        }

        Pipe<double[]>.PipeEndpoint p;
        public void action(Pipe<double[]>.PipeEndpoint p)
        {
            this.p = p;
            RobotRaconteurNode.s().postToThreadPool(this);
        }
    }

    class p1_packet_ack_received implements Action2<Pipe<double[]>.PipeEndpoint, Long>
    {
        public void action(Pipe<double[]>.PipeEndpoint p, Long packetnum1)
        {
            if (packetnum1 == packetnum)
            {
                ack_recv = true;
            }
        }
    }

    private Pipe<teststruct2> m_p2;
    public final Pipe<teststruct2> get_p2()
    {
        return m_p2;
    }
    public final void set_p2(Pipe<teststruct2> value)
    {
        m_p2 = (Pipe<teststruct2>)value;
        m_p2.setPipeConnectCallback(new p2_connect_callback());
    }

    class p2_connect_callback implements Action1<Pipe<teststruct2>.PipeEndpoint>
    {
        public void action(Pipe<teststruct2>.PipeEndpoint p)
        {
            p.addPacketReceivedListener(new p2_packet_received());
        }
    }

    class p2_packet_received implements Action1<Pipe<teststruct2>.PipeEndpoint>, Action
    {
        Pipe<teststruct2>.PipeEndpoint p;
        private Object p2_lock = new Object();
        public void action(Pipe<teststruct2>.PipeEndpoint p)
        {
            this.p = p;
            RobotRaconteurNode.s().postToThreadPool(this);
        }
        public void action()
        {
            try
            {
                Thread.sleep(500);
            }
            catch (Exception e)
            {}
            synchronized (p2_lock)
            {
                while (p.available() > 0)
                {
                    teststruct2 dat = p.receivePacket();
                    p.sendPacket(dat);
                }
            }
        }
    }
    public final void pipe_check_error()
    {
        RRAssert.isTrue(ack_recv);
    }

    public final void test_callbacks()
    {
        get_cb1().getClientFunction(ServerEndpoint.getCurrentEndpoint()).action();
        get_cb2().getClientFunction(ServerEndpoint.getCurrentEndpoint()).action(739.2, 0.392);
        double res = get_cb3().getClientFunction(ServerEndpoint.getCurrentEndpoint()).func(34.0, 45.0);
        RRAssert.areEqual(res, (34 + 45 + 3.14));
        RRAssert.areEqual(
            get_cb_meaning_of_life().getClientFunction(ServerEndpoint.getCurrentEndpoint()).func().longValue(), 42);

        boolean errthrown = false;

        try
        {
            get_cb_errtest().getClientFunction(ServerEndpoint.getCurrentEndpoint()).action();
        }
        catch (Exception e)
        {
            errthrown = true;
        }

        RRAssert.isTrue(errthrown);
    }

    private Callback<Action> m_cb1;
    public final Callback<Action> get_cb1()
    {
        return m_cb1;
    }
    public final void set_cb1(Callback<Action> value)
    {
        m_cb1 = value;
    }

    private Callback<Action2<Double, Double>> m_cb2;
    public final Callback<Action2<Double, Double>> get_cb2()
    {
        return m_cb2;
    }
    public final void set_cb2(Callback<Action2<Double, Double>> value)
    {
        m_cb2 = value;
    }

    private Callback<Func2<Double, Double, Double>> m_cb3;
    public final Callback<Func2<Double, Double, Double>> get_cb3()
    {
        return m_cb3;
    }
    public final void set_cb3(Callback<Func2<Double, Double, Double>> value)
    {
        m_cb3 = value;
    }

    private Callback<Func<Integer>> m_cb_meaning_of_life;
    public final Callback<Func<Integer>> get_cb_meaning_of_life()
    {
        return m_cb_meaning_of_life;
    }
    public final void set_cb_meaning_of_life(Callback<Func<Integer>> value)
    {
        m_cb_meaning_of_life = value;
    }

    private Callback<Action> m_cb_errtest;
    public final Callback<Action> get_cb_errtest()
    {
        return m_cb_errtest;
    }
    public final void set_cb_errtest(Callback<Action> value)
    {
        m_cb_errtest = value;
    }

    private Wire<double[]> m_w1;
    public final Wire<double[]> get_w1()
    {
        return m_w1;
    }
    public final void set_w1(Wire<double[]> value)
    {
        m_w1 = value;
        m_w1.setWireConnectCallback(new w1_connect_callback());
    }
    class w1_connect_callback implements Action2<Wire<double[]>, Wire<double[]>.WireConnection>
    {
        public void action(Wire<double[]> wire, Wire<double[]>.WireConnection con)
        {
            con.addWireValueListener(new w1_value_changed());
        }
    }

    class w1_value_changed implements Action3<Wire<double[]>.WireConnection, double[], TimeSpec>
    {
        public void action(Wire<double[]>.WireConnection con, double[] val, TimeSpec time)
        {

            con.setOutValue(con.getInValue());
        }
    }

    private Wire<teststruct2> m_w2;
    public final Wire<teststruct2> get_w2()
    {
        return m_w2;
    }
    public final void set_w2(Wire<teststruct2> value)
    {
        m_w2 = value;
        m_w2.setWireConnectCallback(new w2_connect_callback());
    }
    class w2_connect_callback implements Action2<Wire<teststruct2>, Wire<teststruct2>.WireConnection>
    {
        public void action(Wire<teststruct2> wire, Wire<teststruct2>.WireConnection con)
        {
            con.addWireValueListener(new w2_value_changed());
        }
    }

    class w2_value_changed implements Action3<Wire<teststruct2>.WireConnection, teststruct2, TimeSpec>
    {
        public void action(Wire<teststruct2>.WireConnection con, teststruct2 val, TimeSpec time)
        {
            con.setOutValue(con.getInValue());
        }
    }

    private Wire<MultiDimArray> m_w3;
    public final Wire<MultiDimArray> get_w3()
    {
        return m_w3;
    }
    public final void set_w3(Wire<MultiDimArray> value)
    {
        m_w3 = value;
        m_w3.setWireConnectCallback(new w3_connect_callback());
    }

    class w3_connect_callback implements Action2<Wire<MultiDimArray>, Wire<MultiDimArray>.WireConnection>
    {

        public void action(Wire<MultiDimArray> wire, Wire<MultiDimArray>.WireConnection con)
        {
            con.addWireValueListener(new w3_value_changed());
        }
    }
    class w3_value_changed implements Action3<Wire<MultiDimArray>.WireConnection, MultiDimArray, TimeSpec>
    {
        public void action(Wire<MultiDimArray>.WireConnection con, MultiDimArray val, TimeSpec time)
        {
            con.setOutValue(con.getInValue());
        }
    }

    private ArrayMemory<double[]> m_m1 = new ArrayMemory<double[]>(TestVectors.getDoubles("ts1.m1.initial"));
    public final ArrayMemory<double[]> get_m1()
    {

        return m_m1;
    }
    public final void set_m1(ArrayMemory<Double> value)
    {
        throw new UnsupportedOperationException();
    }
    private MultiDimArrayMemory<double[]> m_m2 =
        new MultiDimArrayMemory<double[]>(new MultiDimArray(new int[] {10, 10, 10, 10, 10}, new double[100000]));
    public final MultiDimArrayMemory<double[]> get_m2()
    {

        return m_m2;
    }
    public final void set_m2(MultiDimArrayMemory<Double> value)
    {
        throw new UnsupportedOperationException();
    }
    private MultiDimArrayMemory<UnsignedBytes> m_m3 = new MultiDimArrayMemory<UnsignedBytes>(
        new MultiDimArray(new int[] {1024, 1024}, new UnsignedBytes(1024 * 1024)));
    public final MultiDimArrayMemory<UnsignedBytes> get_m3()
    {

        return m_m3;
    }
    public final void set_m3(MultiDimArrayMemory<Byte> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public MultiDimArray get_d6()
    {
        double[] r = new double[] {4.427272e-10,  -1.149547e-08, -1.134096e+16, -4.932974e-03, -7.702447e-01,
                                   -3.468374e-03, -5.037849e-14, -4.140513e-08, 4.553774e+03};

        int[] dims = new int[] {3, 3};
        return new MultiDimArray(dims, r);
    }
    @Override public void set_d6(MultiDimArray value)
    {
        double[] r = new double[] {-5.528040e-08, 3.832644e-01, -9.139211e-22, -4.919312e-05, 3.809620e-11,
                                   1.751983e-09,  2.207872e-21, 1.432794e+09,  -1.970313e+11};

        int[] dims = new int[] {3, 3};

        ca(dims, value.dims);
        ca(r, (double[])value.array);
    }
    @Override public List<double[]> get_list_d1()
    {
        java.util.List<double[]> o = new java.util.ArrayList<double[]>();
        o.add(new double[] {1.643392e-01});
        o.add(new double[] {1.537133e+09});
        o.add(new double[] {1.369505e-03});
        return o;
    }
    @Override public void set_list_d1(List<double[]> value)
    {
        RRAssert.areEqual(value.size(), 3);
        RRAssert.areEqual(value.get(0)[0], 4.074501e-07);
        RRAssert.areEqual(value.get(1)[0], -4.535303e+05);
        RRAssert.areEqual(value.get(2)[0], -2.956241e-20);
    }
    @Override public List<double[]> get_list_d3()
    {
        java.util.ArrayList<double[]> o = new java.util.ArrayList<double[]>();
        o.add(new double[] {8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                            2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                            -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                            -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01});
        o.add(new double[] {4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                            4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                            2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                            1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02});
        return o;
    }
    @Override public void set_list_d3(List<double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get(0), new double[] {4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                       -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                       4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                       -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15});
        ca(value.get(1), new double[] {3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                       7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                       5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                       -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05});
    }
    @Override public List<MultiDimArray> get_list_d5()
    {
        MultiDimArray a = new MultiDimArray(new int[] {10, 10}, TestVectors.getDoubles("ts1.is_d5.to_client"));
        java.util.ArrayList<MultiDimArray> o = new java.util.ArrayList<MultiDimArray>();
        o.add(a);
        return o;
    }
    @Override public void set_list_d5(List<MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get(0);
        ca(a.dims, new int[] {10, 20});
        ca((double[])a.array, TestVectors.getDoubles("ts1.is_d5.to_service"));
    }
    @Override public List<String> get_list_str1()
    {
        java.util.List<String> o = new java.util.ArrayList<String>();
        o.add("Hello server");
        return o;
    }
    @Override public void set_list_str1(List<String> value)
    {
        RRAssert.areEqual(value.size(), 1);
        RRAssert.areEqual(value.get(0), "Hello client");
    }
    @Override public List<teststruct2> get_list_struct1()
    {
        java.util.List<teststruct2> o = new java.util.ArrayList<teststruct2>();
        teststruct2 s = new teststruct2();
        s.mydat = new double[] {-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                                -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                                -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                                -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21};
        o.add(s);
        return o;
    }
    @Override public void set_list_struct1(List<teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get(0);
        ca(s.mydat, new double[] {-2.101948e-07, -2.594836e-08, 2.515710e+01,  -3.834127e-14, -3.088095e+06,
                                  -3.256612e-02, -1.855481e-19, 3.801916e+07,  2.145894e+09,  4.487676e+12,
                                  1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00, 1.786029e-20,
                                  -4.991515e+08, 4.006107e-10,  -4.947871e-11, -2.737020e-08, 4.123759e-20});
    }
    @Override public void func_errtest1()
    {
        throw new DataTypeException("This is a test");
    }
    @Override public void func_errtest2()
    {
        throw new testexception1("This is a test");
    }
    @Override public void func_errtest3()
    {
        throw new testexception3("This is a test");
    }

    Pipe<double[]> m_broadcastpipe = null;
    PipeBroadcaster<double[]> m_broadcastpipe_broadcaster;

    @Override public Pipe<double[]> get_broadcastpipe()
    {
        return m_broadcastpipe;
    }
    @Override public void set_broadcastpipe(Pipe<double[]> value)
    {
        m_broadcastpipe = value;
        m_broadcastpipe_broadcaster = new PipeBroadcaster<double[]>(value, 3);
        RRAssert.areEqual(m_broadcastpipe_broadcaster.getMaxBacklog(), 3);
        m_broadcastpipe_broadcaster.setMaxBacklog(3);

        BroadcastPipeTimerHandler h = new BroadcastPipeTimerHandler();
        com.robotraconteur.Timer t = RobotRaconteurNode.s().createTimer(40, h, false);

        t.start();
    }

    class empty_handler implements Action
    {

        @Override public void action()
        {
            return;
        }
    }

    class BroadcastPipeTimerHandler implements Action1<TimerEvent>
    {

        @Override public void action(TimerEvent p1)
        {
            for (double i = 0; i < 100; i++)
            {
                m_broadcastpipe_broadcaster.asyncSendPacket(new double[] {i}, new empty_handler());
            }
        }
    }

    Wire<double[]> m_broadcastwire;
    WireBroadcaster<double[]> m_broadcastwire_broadcaster;

    @Override public Wire<double[]> get_broadcastwire()
    {

        return m_broadcastwire;
    }
    @Override public void set_broadcastwire(Wire<double[]> value)
    {
        RRAssert.areEqual(m_broadcastwire, null);
        m_broadcastwire = value;
        m_broadcastwire_broadcaster = new WireBroadcaster<double[]>(value);

        BroadcastWireTimerHandler h = new BroadcastWireTimerHandler();
        com.robotraconteur.Timer t = RobotRaconteurNode.s().createTimer(40, h, false);

        t.start();
    }

    class BroadcastWireTimerHandler implements Action1<TimerEvent>
    {

        @Override public void action(TimerEvent p1)
        {
            try
            {
                for (double i = 0; i < 100; i++)
                {
                    m_broadcastwire_broadcaster.setOutValue(new double[] {i});
                }
            }
            catch (Exception e)
            {}
        }
    }
}

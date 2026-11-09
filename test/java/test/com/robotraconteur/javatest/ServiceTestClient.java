package com.robotraconteur.javatest;

import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

import java.util.*;

import com.robotraconteur.*;
import com.robotraconteur.Pipe.PipeEndpoint;

import java.util.concurrent.*;

// import java.nio.file.*;

// cSpell: ignore lstruct, myind, testmdarray

public class ServiceTestClient
{
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

    public final void RunFullTest(String url, String authurl)
    {

        connectService(url);
        TestProperties();
        TestFunctions();
        TestEvents();
        TestObjRefs();
        TestPipes();
        TestCallbacks();
        TestWires();
        TestMemories();

        disconnectService();

        TestAuthentication(authurl);
        TestObjectLock(authurl);
        TestMonitorLock(url);

        TestAsync(authurl);
    }

    private testroot r;

    public final void connectService(String url)
    {

        r = (testroot)RobotRaconteurNode.s().connectService(url);
        Map<String, Object> attrib = RobotRaconteurNode.s().getServiceAttributes(r);
        System.out.println(attrib);
    }

    public final void disconnectService()
    {
        RobotRaconteurNode.s().disconnectService(r);
    }

    public final void TestProperties()
    {
        TestProperties1();
        TestProperties2();
        TestProperties3();
    }

    public final void TestProperties1()
    {
        r.set_d1(3.456);
        RRAssert.areEqual(r.get_d1(), 12.345);
        r.set_d2(TestVectors.getDoubles("ts1.d2.to_service"));
        ca(r.get_d2(), TestVectors.getDoubles("ts1.d2.to_client"));
        r.set_d3(new double[] {9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01,
                               1.969554e+12, 9.451979e-05, 5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13,
                               2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14});
        ca(r.get_d3(), new double[] {2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11,
                                     9.818531e-21, 6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00,
                                     3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21});
        r.set_d4(new double[] {-4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11, 3.511191e-08,
                               4.759250e-07, 2.455883e+09, 4.182578e+11, 4.732337e-14, -2.967313e+02, -4.139188e+14,
                               6.287269e+03});
        ca(r.get_d4(),
           new double[] {2.864760e-08, 3.900663e+13, 9.105789e+11, 2.943743e-15, -2.823159e-16, -3.481261e+19});

        MultiDimArray d5_1 = new MultiDimArray(new int[] {8, 6, 2}, TestVectors.getDoubles("ts1.d5.to_service"));

        r.set_d5(d5_1);

        MultiDimArray d5_2 = r.get_d5();
        ca(d5_2.dims, new int[] {5, 6, 5});
        ca((double[])d5_2.array, TestVectors.getDoubles("ts1.d5.to_client"));

        MultiDimArray d6_1 = new MultiDimArray(
            new int[] {3, 3}, new double[] {-5.528040e-08, 3.832644e-01, -9.139211e-22, -4.919312e-05, 3.809620e-11,
                                            1.751983e-09, 2.207872e-21, 1.432794e+09, -1.970313e+11});

        r.set_d6(d6_1);

        MultiDimArray d6_2 = r.get_d6();
        ca(d6_2.dims, new int[] {3, 3});
        ca((double[])d6_2.array, new double[] {4.427272e-10, -1.149547e-08, -1.134096e+16, -4.932974e-03, -7.702447e-01,
                                               -3.468374e-03, -5.037849e-14, -4.140513e-08, 4.553774e+03});

        r.set_s1(3847.9283f);
        RRAssert.areEqual(r.get_s1(), 7.8573f);
        r.set_s2(TestVectors.getFloats("ts1.s2.to_service"));
        ca(r.get_s2(), TestVectors.getFloats("ts1.s2.to_client"));

        r.set_i8_1((byte)45);
        RRAssert.areEqual(r.get_i8_1(), -66);
        r.set_i8_2(TestVectors.getBytes("ts1.i8_2.to_service"));
        ca(r.get_i8_2(), TestVectors.getBytes("ts1.i8_2.to_client"));

        r.set_u8_1(new UnsignedByte((byte)-24));
        RRAssert.areEqual(r.get_u8_1().value, -34);
        r.set_u8_2(new UnsignedBytes(TestVectors.getUInt8s("ts1.u8_2.to_service")));
        ca(r.get_u8_2().value, TestVectors.getUInt8s("ts1.u8_2.to_client"));

        MultiDimArray u8_3_1 =
            new MultiDimArray(new int[] {30, 10}, new UnsignedBytes(TestVectors.getUInt8s("ts1.u8_3.to_service")));
        r.set_u8_3(u8_3_1);
        MultiDimArray u8_3_2 = r.get_u8_3();
        ca(new int[] {10, 20}, u8_3_2.dims);
        ca(((UnsignedBytes)u8_3_2.array).value, TestVectors.getUInt8s("ts1.u8_3.to_client"));

        r.set_i16_1((short)2387);
        RRAssert.areEqual(r.get_i16_1(), -13428);
        r.set_i16_2(TestVectors.getShorts("ts1.i16_2.to_service"));
        ca(r.get_i16_2(), TestVectors.getShorts("ts1.i16_2.to_client"));

        r.set_u16_1(new UnsignedShort((short)-10804));
        RRAssert.areEqual(r.get_u16_1().value, -4555);
        r.set_u16_2(new UnsignedShorts(TestVectors.getUInt16s("ts1.u16_2.to_service")));
        ca(r.get_u16_2().value, TestVectors.getUInt16s("ts1.u16_2.to_client"));
    }

    public final void TestProperties2()

    {
        r.set_i32_1(-9837284);
        RRAssert.areEqual(r.get_i32_1(), 898734);
        r.set_i32_2(TestVectors.getInts("ts1.i32_2.to_service"));
        ca(r.get_i32_2(), TestVectors.getInts("ts1.i32_2.to_client"));

        {
            int[] i32_huge_1 = new int[2621440];
            for (int i = 0; i < 2621440; i++)
            {
                i32_huge_1[i] = -i;
            }
            r.set_i32_huge(i32_huge_1);
        }
        {
            int[] i32_huge_2 = r.get_i32_huge();
            RRAssert.areEqual(i32_huge_2.length, 2621440);
            for (int i = 0; i < 2621440; i++)
            {
                RRAssert.areEqual(i32_huge_2[i], i);
            }
        }

        r.set_u32_1(new UnsignedInt(1550099195));
        RRAssert.areEqual(r.get_u32_1().value, (547919123));
        r.set_u32_2(new UnsignedInts(TestVectors.getUInt32s("ts1.u32_2.to_service")));
        ca(r.get_u32_2().value, TestVectors.getUInt32s("ts1.u32_2.to_client"));

        r.set_i64_1(8621740821050813024L);
        RRAssert.areEqual(r.get_i64_1(), -1357833931563696072L);
        r.set_i64_2(TestVectors.getLongs("ts1.i64_2.to_service"));
        ca(r.get_i64_2(), TestVectors.getLongs("ts1.i64_2.to_client"));

        r.set_u64_1(new UnsignedLong(1465640522145789825L));
        RRAssert.areEqual(r.get_u64_1().value, -5056882102845907238L);
        r.set_u64_2(new UnsignedLongs(TestVectors.getUInt64s("ts1.u64_2.to_service")));
        ca(r.get_u64_2().value, TestVectors.getUInt64s("ts1.u64_2.to_client"));

        r.set_str1("Hello Server!");
        RRAssert.areEqual(r.get_str1(), "Hello Client!");

        // read in and check the large structure struct1
        teststruct1 s1 = r.get_struct1();
        ca(s1.dat1, TestVectors.getDoubles("ts1.struct1.dat1.to_client"));
        RRAssert.areEqual(s1.str2, "Hello world!");
        RRAssert.areEqual(s1.vec3.size(), 3);
        RRAssert.areEqual(s1.vec3.get(1), "Hello Client!");
        RRAssert.areEqual(s1.vec3.get(2), "Hello Client, again");
        RRAssert.areEqual(s1.vec3.get(4372), "This is yet another test string");

        RRAssert.areEqual(s1.dict4.size(), 3);
        RRAssert.areEqual(s1.dict4.get("teststring1"), "Hello Client!");
        RRAssert.areEqual(s1.dict4.get("teststring2"), "Hello Client, again");
        RRAssert.areEqual(s1.dict4.get("anotherstr"), "This is yet another test string");

        RRAssert.areEqual(s1.list5.size(), 3);
        RRAssert.areEqual(s1.list5.get(0), "Hello Client!");
        RRAssert.areEqual(s1.list5.get(1), "Hello Client, again");
        RRAssert.areEqual(s1.list5.get(2), "This is yet another test string");

        ca(s1.struct1.mydat, TestVectors.getDoubles("ts1.struct1.struct1_mydat.to_client"));

        RRAssert.areEqual(s1.dstruct2.size(), 2);
        ca(s1.dstruct2.get("test1").mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client"));
        ca(s1.dstruct2.get("anothertest").mydat,
           new double[] {4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08,
                         -4.969544e-21, 2.530419e-01, 4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17,
                         1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02});

        RRAssert.areEqual(s1.lstruct3.size(), 2);
        ca(s1.lstruct3.get(0).mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client"));
        ca(s1.lstruct3.get(1).mydat,
           new double[] {4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08,
                         -4.969544e-21, 2.530419e-01, 4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17,
                         1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02});

        ca(s1.multidimarray.dims, new int[] {10, 10});
        ca((double[])s1.multidimarray.array, TestVectors.getDoubles("ts1.struct1.multidimarray.to_client"));

        RRAssert.areEqual(((String)s1.var3), "This is a vartype string");
    }
    public void TestProperties3()
    {
        // Set large structure for struct1
        teststruct1 s2 = new teststruct1();
        s2.dat1 = TestVectors.getDoubles("ts1.struct1.dat1.to_service");
        s2.str2 = "Hello world 2!";
        java.util.HashMap<Integer, String> s2_3 = new java.util.HashMap<Integer, String>();
        s2_3.put(10, "Hello Server!");
        s2_3.put(11, "Hello Server, again");
        s2_3.put(46372, "Test string!");
        s2_3.put(46373, "Test string again");

        s2.vec3 = s2_3;

        java.util.HashMap<String, String> s2_4 = new java.util.HashMap<String, String>();
        s2_4.put("cteststring1", "Hello Server!");
        s2_4.put("cteststring2", "Hello Server, again");
        s2.dict4 = s2_4;

        java.util.List<String> s2_5 = new java.util.ArrayList<String>();
        s2_5.add("Hello Server!");
        s2_5.add("Hello Server, again");
        s2.list5 = s2_5;

        s2.struct1 = new teststruct2();
        s2.struct1.mydat = TestVectors.getDoubles("ts1.struct1.struct1_mydat.to_service");

        java.util.HashMap<String, teststruct2> s2_ds2 = new java.util.HashMap<String, teststruct2>();
        teststruct2 s2_ds2_1 = new teststruct2();
        s2_ds2_1.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_service");
        s2_ds2.put("ctest1", s2_ds2_1);
        teststruct2 s2_ds2_2 = new teststruct2();
        s2_ds2_2.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_2.to_service");
        s2_ds2.put("anothertest", s2_ds2_2);
        s2.dstruct2 = s2_ds2;

        java.util.List<teststruct2> s2_ls3 = new java.util.ArrayList<teststruct2>();
        teststruct2 s2_ls3_1 = new teststruct2();
        s2_ls3_1.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_service");
        s2_ls3.add(s2_ds2_1);
        teststruct2 s2_ls3_2 = new teststruct2();
        s2_ls3_2.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_2.to_service");
        s2_ls3.add(s2_ds2_2);
        s2.lstruct3 = s2_ls3;

        s2.multidimarray =
            new MultiDimArray(new int[] {10, 10}, TestVectors.getDoubles("ts1.struct1.multidimarray.to_service"));
        s2.var3 = new double[] {6.404176e-12,  9.258110e-03,  8.657620e-03, -2.064381e+00, 5.182360e-16,
                                4.167658e-16,  -4.533051e-19, 5.357520e+18, -4.990383e-13, 2.286982e+08,
                                -4.727256e-18, 1.465299e-17,  3.000340e-10, -2.304453e-04};
        r.set_struct1(s2);

        // Check struct2 in/out
        teststruct2 s3 = new teststruct2();
        s3.mydat = TestVectors.getDoubles("ts1.struct2.to_service");
        r.set_struct2(s3);
        teststruct2 s4 = r.get_struct2();
        ca(s4.mydat, TestVectors.getDoubles("ts1.struct2.to_client"));

        // Test is_d1
        java.util.Map<Integer, double[]> is_d1_1 = r.get_is_d1();
        RRAssert.areEqual(is_d1_1.size(), 3);
        RRAssert.areEqual(is_d1_1.get(9285)[0], 1.643392e-01);
        RRAssert.areEqual(is_d1_1.get(74822)[0], 1.537133e+09);
        RRAssert.areEqual(is_d1_1.get(4)[0], 1.369505e-03);

        java.util.HashMap<Integer, double[]> is_d1_2 = new java.util.HashMap<Integer, double[]>();
        is_d1_2.put(928, new double[] {4.074501e-07});
        is_d1_2.put(394820, new double[] {-4.535303e+05});
        is_d1_2.put(623, new double[] {-2.956241e-20});
        r.set_is_d1(is_d1_2);

        // Test is_d2
        Map<String, double[]> is_d2_1 = r.get_is_d2();
        RRAssert.areEqual(is_d2_1.size(), 2);
        RRAssert.areEqual(is_d2_1.get("testval1")[0], -1.079664e+16);
        RRAssert.areEqual(is_d2_1.get("testval2")[0], 2.224846e+00);

        java.util.HashMap<String, double[]> is_d2_2 = new java.util.HashMap<String, double[]>();
        is_d2_2.put("testval3", new double[] {5.242474e+10});
        is_d2_2.put("testval4", new double[] {2.208636e+08});
        r.set_is_d2(is_d2_2);

        // Test is_d3

        java.util.Map<Integer, double[]> is_d3_1 = r.get_is_d3();
        RRAssert.areEqual(is_d3_1.size(), 2);
        ca(is_d3_1.get(12), new double[] {8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                                          2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                                          -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                                          -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01});
        ca(is_d3_1.get(832), new double[] {4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                                           4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                                           2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                                           1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02});

        java.util.HashMap<Integer, double[]> is_d3_2 = new java.util.HashMap<Integer, double[]>();
        is_d3_2.put(47, new double[] {4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                      -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                      4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                      -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15});
        is_d3_2.put(324, new double[] {3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                       7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                       5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                       -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05});
        r.set_is_d3(is_d3_2);

        // Test is_d4

        java.util.Map<String, double[]> is_d4_1 = r.get_is_d4();
        RRAssert.areEqual(is_d4_1.size(), 2);
        ca(is_d4_1.get("testval1"),
           new double[] {1.113851e-04,  3.830104e+07, 4.571169e-21,  -4.064180e-05, 2.889736e+01,
                         -1.790060e-06, 4.608538e+00, 4.687713e-04,  1.387717e-08,  3.914187e-18,
                         -5.618118e-06, 1.530811e+05, -5.848922e-11, -3.397558e-20, -6.597368e-08,
                         -3.779049e-06, 2.406033e-19, 2.507939e-10,  3.246113e-20,  1.341205e+16});
        ca(is_d4_1.get("testval2"),
           new double[] {-3.088190e-13, -4.033334e-20, 4.150103e-21,  -6.610855e+17, 3.688824e-13,
                         -3.208025e+13, -5.034888e-11, -4.098363e-06, -1.272830e-03, 2.748392e-03,
                         -2.644272e-06, -4.810065e-18, 4.629861e-19,  -5.444015e-03, 4.046008e+17,
                         -3.548079e+12, -3.455290e+16, -3.668946e-12, -3.522178e-01, -1.537583e+14});

        java.util.HashMap<String, double[]> is_d4_2 = new java.util.HashMap<String, double[]>();
        is_d4_2.put("testval3", new double[] {1.771838e+06, 3.037284e-01,  -1.739742e-02, 1.399508e-20,  3.605232e-21,
                                              3.517522e+14, 4.887514e+14,  3.505442e-03,  -3.968972e+18, 1.422037e-20,
                                              2.596937e-21, 4.852833e-11,  6.852955e-17,  4.765526e-12,  -3.445954e+16,
                                              2.322531e-14, -1.755122e-12, 3.941875e+00,  8.877046e-13,  2.818923e-02});
        is_d4_2.put("testval4",
                    new double[] {4.146439e+16,  2.923439e-07,  3.549608e+16,  -1.664891e-01, -4.192309e-15,
                                  3.857317e+05,  -1.101076e+00, 1.213105e+19,  3.237584e-14,  -2.421219e-06,
                                  -4.603196e-05, -3.719535e-10, 1.124961e+06,  2.032849e+10,  4.639704e-22,
                                  3.946835e+01,  -9.267263e+01, -4.456188e+11, 3.470487e+08,  7.918764e+10});
        r.set_is_d4(is_d4_2);

        // Test is_d5
        java.util.Map<Integer, MultiDimArray> is_d5_1 = r.get_is_d5();
        RRAssert.areEqual(is_d5_1.size(), 1);
        MultiDimArray is_d5_1_1 = is_d5_1.get(564);
        ca(is_d5_1_1.dims, new int[] {10, 10});
        ca((double[])is_d5_1_1.array, TestVectors.getDoubles("ts1.is_d5.to_client"));

        MultiDimArray is_d5_2_1 = new MultiDimArray(new int[] {10, 20}, TestVectors.getDoubles("ts1.is_d5.to_service"));
        java.util.HashMap<Integer, MultiDimArray> is_d5_2 = new java.util.HashMap<Integer, MultiDimArray>();
        is_d5_2.put(328, is_d5_2_1);

        // Test is_d6
        java.util.Map<String, MultiDimArray> is_d6_1 = r.get_is_d6();
        RRAssert.areEqual(is_d6_1.size(), 1);
        MultiDimArray is_d6_1_1 = is_d6_1.get("testval1");
        ca(is_d6_1_1.dims, new int[] {5, 10});
        ca((double[])is_d6_1_1.array, TestVectors.getDoubles("ts1.is_d6.to_client"));

        MultiDimArray is_d6_2_1 = new MultiDimArray(new int[] {8, 10}, TestVectors.getDoubles("ts1.is_d6.to_service"));
        java.util.HashMap<String, MultiDimArray> is_d6_2 = new java.util.HashMap<String, MultiDimArray>();
        is_d6_2.put("testval2", is_d6_2_1);
        r.set_is_d6(is_d6_2);

        // Test is_str1
        java.util.Map<Integer, String> is_str1_1 = r.get_is_str1();
        RRAssert.areEqual(is_str1_1.size(), 1);
        RRAssert.areEqual(is_str1_1.get(23), "Hello server");

        java.util.HashMap<Integer, String> is_str1_2 = new java.util.HashMap<Integer, String>();
        is_str1_2.put(24, "Hello client");
        r.set_is_str1(is_str1_2);

        // Test is_str2
        java.util.Map<String, String> is_str2_1 = r.get_is_str2();
        RRAssert.areEqual(is_str2_1.size(), 1);
        RRAssert.areEqual(is_str2_1.get("testval1"), "Hello server");

        java.util.HashMap<String, String> is_str2_2 = new java.util.HashMap<String, String>();
        is_str2_2.put("testval2", "Hello client");
        r.set_is_str2(is_str2_2);

        // Test is_struct1
        java.util.Map<Integer, teststruct2> is_struct1_1 = r.get_is_struct1();
        ca(is_struct1_1.get(748).mydat,
           new double[] {-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                         -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                         -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                         -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21});

        java.util.HashMap<Integer, teststruct2> is_struct1_2 = new java.util.HashMap<Integer, teststruct2>();
        teststruct2 is_struct1_2_1 = new teststruct2();
        is_struct1_2_1.mydat = new double[] {-2.101948e-07, -2.594836e-08, 2.515710e+01,  -3.834127e-14, -3.088095e+06,
                                             -3.256612e-02, -1.855481e-19, 3.801916e+07,  2.145894e+09,  4.487676e+12,
                                             1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00, 1.786029e-20,
                                             -4.991515e+08, 4.006107e-10,  -4.947871e-11, -2.737020e-08, 4.123759e-20};
        is_struct1_2.put(372, is_struct1_2_1);
        r.set_is_struct1(is_struct1_2);

        // Test is_struct2
        java.util.Map<String, teststruct2> is_struct2_1 = r.get_is_struct2();
        ca(is_struct2_1.get("testval1").mydat,
           new double[] {-4.489570e+13, 9.574895e-05,  4.081711e+06,  5.612839e-18,  -1.078604e+05,
                         3.658139e+08,  -4.748975e+05, -2.606481e+01, 3.016739e+15,  3.174709e+19,
                         -4.572549e+17, 1.980389e-04,  -3.551911e-10, 3.598401e-07,  2.659416e-12,
                         -3.606157e+06, 2.059674e+17,  -9.362336e-20, -3.299256e+17, -2.245745e+16});

        java.util.HashMap<String, teststruct2> is_struct2_2 = new java.util.HashMap<String, teststruct2>();
        teststruct2 is_struct2_2_1 = new teststruct2();
        is_struct2_2_1.mydat = new double[] {6.931327e-21,  4.527137e-02,  1.260822e-18,  3.592805e-12,  1.088317e-05,
                                             3.305865e+03,  -9.798828e-20, 1.497504e+18,  -3.653592e+01, 1.473952e+10,
                                             -1.003612e-20, 1.302159e+18,  -8.544326e+05, 1.038521e+16,  -2.845746e-18,
                                             -3.899909e-04, 4.785560e-02,  -7.203365e-12, -1.500022e-14, -1.892753e-17};
        is_struct2_2.put("testval2", is_struct2_2_1);
        r.set_is_struct2(is_struct2_2);

        // Test list_d1
        java.util.List<double[]> list_d1_1 = r.get_list_d1();
        RRAssert.areEqual(list_d1_1.size(), 3);
        RRAssert.areEqual(list_d1_1.get(0)[0], 1.643392e-01);
        RRAssert.areEqual(list_d1_1.get(1)[0], 1.537133e+09);
        RRAssert.areEqual(list_d1_1.get(2)[0], 1.369505e-03);

        java.util.List<double[]> list_d1_2 = new java.util.ArrayList<double[]>();
        list_d1_2.add(new double[] {4.074501e-07});
        list_d1_2.add(new double[] {-4.535303e+05});
        list_d1_2.add(new double[] {-2.956241e-20});
        r.set_list_d1(list_d1_2);

        // Test is_d3

        java.util.List<double[]> list_d3_1 = r.get_list_d3();
        RRAssert.areEqual(list_d3_1.size(), 2);
        ca(list_d3_1.get(0), new double[] {8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                                           2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                                           -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                                           -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01});
        ca(list_d3_1.get(1), new double[] {4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                                           4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                                           2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                                           1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02});

        java.util.List<double[]> list_d3_2 = new java.util.ArrayList<double[]>();
        list_d3_2.add(new double[] {4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                    -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                    4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                    -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15});
        list_d3_2.add(new double[] {3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                    7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                    5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                    -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05});
        r.set_list_d3(list_d3_2);

        // Test list_d5
        java.util.List<MultiDimArray> list_d5_1 = r.get_list_d5();
        RRAssert.areEqual(list_d5_1.size(), 1);
        MultiDimArray list_d5_1_1 = list_d5_1.get(0);
        ca(list_d5_1_1.dims, new int[] {10, 10});
        ca((double[])list_d5_1_1.array, TestVectors.getDoubles("ts1.is_d5.to_client"));

        MultiDimArray list_d5_2_1 =
            new MultiDimArray(new int[] {10, 20}, TestVectors.getDoubles("ts1.is_d5.to_service"));
        java.util.List<MultiDimArray> list_d5_2 = new java.util.ArrayList<MultiDimArray>();
        list_d5_2.add(list_d5_2_1);
        r.set_list_d5(list_d5_2);

        // Test list_str1
        java.util.List<String> list_str1_1 = r.get_list_str1();
        RRAssert.areEqual(list_str1_1.size(), 1);
        RRAssert.areEqual(list_str1_1.get(0), "Hello server");

        java.util.List<String> list_str1_2 = new java.util.ArrayList<String>();
        list_str1_2.add("Hello client");
        r.set_list_str1(list_str1_2);

        // Test list_struct1
        java.util.List<teststruct2> list_struct1_1 = r.get_list_struct1();
        ca(list_struct1_1.get(0).mydat,
           new double[] {-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                         -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                         -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                         -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21});

        java.util.List<teststruct2> list_struct1_2 = new java.util.ArrayList<teststruct2>();
        teststruct2 list_struct1_2_1 = new teststruct2();
        list_struct1_2_1.mydat = new double[] {
            -2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06, -3.256612e-02, -1.855481e-19,
            3.801916e+07,  2.145894e+09,  4.487676e+12, 1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00,
            1.786029e-20,  -4.991515e+08, 4.006107e-10, -4.947871e-11, -2.737020e-08, 4.123759e-20};
        list_struct1_2.add(is_struct1_2_1);
        r.set_list_struct1(list_struct1_2);

        // Test struct3
        ca(r.get_struct3().a1, new double[] {-8.483090e-19, -4.401548e-08, 3.908118e+00,  2.063513e-18,  4.237047e+18,
                                             -1.124681e-16, 3.924541e-01,  -2.184335e-10, -1.978950e+11, 1.586365e+18,
                                             1.712393e+00,  -6.314723e+00, 1.196777e-16,  -2.748704e-08, -1.289967e+02,
                                             -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06,  1.315398e+17});

        ostruct2 struct3_1 = new ostruct2();
        struct3_1.a1 = new double[] {-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03,
                                     8.256717e-12,  -4.772119e-11, -1.061407e-13, 2.759750e+02,  -1.212549e+10,
                                     7.012690e+15,  3.953354e+04,  -2.617985e-07, 1.104408e-21,  -3.889366e+00,
                                     4.549493e+16,  -1.376791e+15, -3.445205e-21, 2.137830e-14,  4.620179e+18};
        r.set_struct3(struct3_1);

        // Test var1  TODO...

        // Test var2 TODO...

        // Test var_num
        ca((int[])r.get_var_num(),
           new int[] {-1680284833, -54562307, 732107275, 1470526962, -1389452949, 256801409, 261288152, 1728150828,
                      1322531658, -1640628174, 1036878614, 511108054, 2057847386, 288780916, 996595759});
        r.set_var_num(new int[] {-1046369769, 1950632347, 1140727074, -1277424443, 163999900, 970815027, 545593183,
                                 514305170, 1896372264, 1385916382});

        // Test var_str
        RRAssert.areEqual(((String)r.get_var_str()), "Hello Client!");
        r.set_var_str("Hello Server!");

        // Test var_struct
        ca(((teststruct2)r.get_var_struct()).mydat,
           new double[] {-9.052731e+13, 4.151705e-17,  -4.004463e+19, -2.838274e+03, 9.983314e+12,
                         2.764122e+10,  -1.131486e+03, 2.418899e+12,  1.323675e-05,  -4.602174e+13,
                         2.717530e+01,  1.193887e-10,  -4.137578e+16, -1.246990e-19, 4.244315e-18,
                         -2.833005e-08, 1.956266e-04,  4.130129e-21,  1.641708e-11,  -4.488158e-19});

        teststruct2 var_struct_1 = new teststruct2();
        var_struct_1.mydat = new double[] {-4.945426e-20, 1.763386e+13,  3.431578e-04,  4.411409e+17,  -2.690201e+03,
                                           3.025939e-10,  -3.659846e+11, -4.780435e-10, -3.246816e+14, -1.815578e+04,
                                           2.236455e+10,  -4.639041e+14, 1.767930e+10,  -1.636094e+05, -4.392462e-01,
                                           2.225260e+04,  -5.250245e+18, 8.755282e-12,  2.005819e-10,  2.702210e+04};
        r.set_var_struct(var_struct_1);

        // Test var_vector
        RRAssert.areEqual(((String)(((java.util.Map<Integer, Object>)r.get_var_vector())).get(10)), "Hello Client!");
        java.util.HashMap<Integer, Object> var_vector_1 = new java.util.HashMap<Integer, Object>();
        var_vector_1.put(11, "Hello Server!");
        r.set_var_vector(var_vector_1);

        // Test var_dictionary
        RRAssert.areEqual(((String)((java.util.HashMap<String, Object>)r.get_var_dictionary()).get("test1")),
                          "Hello Client!");
        java.util.HashMap<String, Object> var_dictionary_1 = new java.util.HashMap<String, Object>();
        var_dictionary_1.put("test2", "Hello Server!");
        r.set_var_dictionary(var_dictionary_1);

        // Test var_vector
        RRAssert.areEqual(((String)(((java.util.List<Object>)r.get_var_list())).get(0)), "Hello Client!");
        java.util.List<Object> var_list_1 = new java.util.ArrayList<Object>();
        var_list_1.add("Hello Server!");
        r.set_var_list(var_list_1);

        // Test var_multidimarray
        MultiDimArray var_a_1 = (MultiDimArray)r.get_var_multidimarray();
        ca(var_a_1.dims, new int[] {5, 4});
        ca((double[])var_a_1.array,
           new double[] {-4.915597e-01, 3.892823e+00,  2.622325e+08,  -7.150935e+04, 9.418756e+00,
                         3.633879e+18,  3.522383e-03,  -4.989811e+05, 2.027383e-03,  -3.153241e+12,
                         -6.948245e-21, -3.198577e+14, 6.172905e+09,  3.849430e+15,  8.600383e+13,
                         4.079437e-17,  3.194775e+06,  4.222550e-18,  1.758122e+17,  -1.018308e+03});
        r.set_var_multidimarray(new MultiDimArray(
            new int[] {5, 4}, new double[] {3.792953e+00,  2.968121e-17,  -3.976413e-15, 4.392986e+19,  2.197463e+10,
                                            -2.627743e-14, -2.184665e+17, 1.972257e-17,  9.929684e-03,  -3.096821e+17,
                                            3.598051e+11,  -6.266015e-18, 1.811985e-11,  2.815232e-07,  7.469467e-06,
                                            6.141798e+13,  3.105763e+09,  -1.697809e-10, -4.141707e-17, 4.391634e+13}));

        // Test errtest and make sure an exception is thrown

        boolean err1 = false;
        try
        {
            r.set_errtest(1);
        }
        catch (Exception e)
        {
            err1 = true;
        }

        RRAssert.isTrue(err1);

        boolean err2 = false;
        try
        {
            double d = r.get_errtest();
        }
        catch (Exception e)
        {
            err2 = true;
        }
        RRAssert.isTrue(err2);

        // Test nulltest
        RRAssert.areEqual(r.get_nulltest(), null);
        r.set_nulltest(null);
    }

    public final void TestFunctions()
    {

        r.func1();
        r.func2(10, 20.34);
        RRAssert.areEqual(r.func3(2, 3.45), 5.45);
        RRAssert.areEqual(r.meaning_of_life(), 42);

        boolean errthrown = false;

        try
        {
            r.func_errtest();
        }
        catch (Exception e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        errthrown = false;
        try
        {
            r.func_errtest1();
        }
        catch (DataTypeException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        errthrown = false;
        try
        {
            r.func_errtest2();
        }
        catch (testexception1 e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        errthrown = false;
        try
        {
            r.func_errtest3();
        }
        catch (testexception3 e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);
    }

    public final void TestEvents()
    {
        System.out.println("Start event");
        ev1_event = new ev1a();
        ev2_event = new ev2a();
        r.addev1Listener(ev1_event);

        r.addev2Listener(ev2_event);

        try
        {

            r.func1();

            synchronized (ev1_event)
            {
                ev1_event.wait(5000);
            }

            r.func2(27.3, 98.23);
            synchronized (ev2_event)
            {
                ev2_event.wait(5000);
            }
        }
        catch (Exception e)
        {
            throw new RuntimeException("Timeout");
        }
        finally
        {

            r.removeev1Listener(ev1_event);

            r.removeev2Listener(ev2_event);

            ev1_event = null;
            ev2_event = null;
        }
    }

    private class ev1a implements Action
    {

        @Override public void action()
        {
            ev1_cb();
        }
    }

    private ev1a ev1_event;
    private void ev1_cb()
    {
        System.out.println("Got ev1");

        ev1_event.notify();
    }

    private class ev2a implements Action2<Double, teststruct2>
    {

        @Override public void action(Double p1, teststruct2 p2)
        {
            ev2_cb(p1.doubleValue(), p2);
        }
    }

    private ev2a ev2_event;
    private void ev2_cb(double d, teststruct2 s)
    {
        System.out.println("Got ev2: " + d + " " + s.mydat[0]);
        if (d == 27.3 && s.mydat[0] == 98.23)
        {

            ev2_event.notify();
        }
    }

    public final void TestObjRefs()
    {
        sub1 o1 = r.get_o1();
        sub1 o2_10 = r.get_o2(10);
        sub1 o2_34 = r.get_o2(34);
        sub1 o3_1 = r.get_o3(1);
        sub1 o4_myind = r.get_o4("myind");
        sub1 o4_specialind = r.get_o4("ind!@#$%^&*().<>     ");

        sub2 o2_10_o2_1_32 = o2_10.get_o2_2(32);

        sub3 o2_10_o2_1_32_o3_ind1 = o2_10_o2_1_32.get_o3_1("ind1");
        sub3 o2_10_o2_1_32_o3_ind2 = o2_10_o2_1_32.get_o3_1("ind2");

        o1.set_d1(new double[] {-2.086627e+06, 3.092642e+04, -1.981667e+02, 1.963286e-20, 4.264052e-08, 3.594320e+12,
                                -4.820517e-02, -3.629590e+06, 6.037089e-07, 3.328419e+06});
        o2_10.set_d1(new double[] {4.978178e-14, 2.867603e-17, 4.471047e-21, -2.002902e+15, -2.910881e-03,
                                   -2.601092e-03, -3.043199e+16, -3.257109e-12, 1.834255e-11, -3.383015e+00});
        o2_34.set_d1(new double[] {4.661927e-02, 2.334444e+02, 3.985567e+12, -2.324843e+01, -3.315866e+03,
                                   -4.442404e+10, 3.280626e-02, 2.334668e-12, -3.374202e-14, 4.809260e+02});
        o3_1.set_d1(new double[] {-1.882441e-04, 2.065458e+14, -6.309214e-16, -3.181637e-07, -9.906616e+02,
                                  1.684926e-14, 1.672252e+15, -3.950901e+01, -3.295950e-17, -3.080902e+13});
        o4_myind.set_d1(new double[] {2.997950e-14, -1.077977e+17, 3.721399e-09, -1.289619e+18, 4.494844e+06,
                                      -4.918719e-15, 2.194759e+13, 2.554572e-09, 4.166299e-06, -1.409589e+04});
        o4_specialind.set_d1(new double[] {3.404179e-08, -1.749189e-18, -3.219593e-09, 1.313794e+01, -4.193673e+10,
                                           -2.479829e+07, -2.617068e+04, 8.181730e+15, -2.003653e+18, -1.833401e+19});
        o2_10_o2_1_32.set_data("Hello world!");
        o2_10_o2_1_32_o3_ind1.set_data2("Test string 1");
        o2_10_o2_1_32_o3_ind2.set_data2("Test string 2");

        // Test the varobject type and make sure the ReleaseServicePath works...

        r.o6_op(0);
        sub1 o6_1 = (sub1)r.get_o6();
        sub2 o6_1_1 = o6_1.get_o2_1();
        o6_1.set_d1(new double[] {0.0});
        o6_1_1.set_data("Hello world!");

        for (int i = 0; i < 10; i++)
        {
            try
            {
                r.o6_op(1);
                try
                {
                    Thread.sleep(100);
                }
                catch (InterruptedException e)
                {}
                boolean err1 = false;
                try
                {
                    o6_1.set_d1(new double[] {0.0});
                }
                catch (Exception ee)
                {
                    err1 = true;
                }
                RRAssert.isTrue(err1);

                sub2 o6_2 = (sub2)r.get_o6();
                o6_2.set_data("Hello world!");

                r.o6_op(2);
                try
                {
                    Thread.sleep(100);
                }
                catch (InterruptedException e)
                {}

                subobj o6_3 = (subobj)r.get_o6();
                o6_3.add_val(2);
                break;
            }
            catch (Exception e)
            {
                if (i == 9)
                {
                    throw e;
                }
                else
                {
                    try
                    {
                        Thread.sleep(1000);
                    }
                    catch (InterruptedException ee)
                    {}
                    continue;
                }
            }
        }
    }

    private ee1a ee1;
    private ee1a_ack ee1_ack;
    private ee2a ee2;
    private ee3a ee3;

    private long packetnum = 0;
    private boolean ack_recv = false;

    public final void TestPipes()
    {

        ee1 = new ee1a();
        ee1_ack = new ee1a_ack();
        ee2 = new ee2a();
        ee3 = new ee3a();

        Pipe<double[]>.PipeEndpoint e1 = r.get_p1().connect(-1);
        Pipe<double[]>.PipeEndpoint e2 = r.get_p1().connect(3432);
        Pipe<teststruct2>.PipeEndpoint e3 = r.get_p2().connect(-1);

        e1.setRequestPacketAck(true);

        e1.addPacketReceivedListener(ee1);
        e1.addPacketAckReceivedListener(ee1_ack);
        e2.addPacketReceivedListener(ee2);
        e3.addPacketReceivedListener(ee3);
        synchronized (this)
        {
            packetnum = e1.sendPacket(new double[] {1, 2, 3, 4});
        }
        e1.sendPacket(new double[] {5, 6, 7, 8});
        e1.sendPacket(new double[] {-1, -2, -3, -5.32});

        e2.sendPacket(new double[] {3.21});
        e2.sendPacket(new double[] {4.72});
        e2.sendPacket(new double[] {72.34});

        teststruct2 s1 = new teststruct2();
        s1.mydat = new double[] {738.29};
        teststruct2 s2 = new teststruct2();
        s2.mydat = new double[] {89.83};

        e3.sendPacket(s1);
        e3.sendPacket(s2);

        try
        {
            synchronized (ee1)
            {
                ee1.wait(5000);
            }
            synchronized (ee2)
            {
                ee2.wait(5000);
            }
            synchronized (ee3)
            {
                ee3.wait(5000);
            }
        }
        catch (Exception e)
        {
            RRAssert.fail();
        }

        ca(e1.receivePacket(), new double[] {1, 2, 3, 4});
        ca(e1.receivePacketWait(1000), new double[] {5, 6, 7, 8});
        ca(e1.receivePacketWait(1000), new double[] {-1, -2, -3, -5.32});

        ca(e2.receivePacket(), new double[] {3.21});
        ca(e2.receivePacketWait(1000), new double[] {4.72});
        ca(e2.receivePacketWait(1000), new double[] {72.34});

        ca(e3.receivePacket().mydat, new double[] {738.29});
        ca(e3.receivePacketWait(1000).mydat, new double[] {89.83});

        for (int i = 0; i < 10; i++)
        {
            try
            {
                RRAssert.isTrue(ack_recv);

                r.pipe_check_error();
                break;
            }
            catch (Exception e)
            {
                if (i == 9)
                {
                    throw e;
                }
                else
                {
                    try
                    {
                        Thread.sleep(1000);
                    }
                    catch (InterruptedException ee)
                    {}
                    continue;
                }
            }
        }

        e1.close();
        e2.close();
        e3.close();
    }

    private class ee1a implements Action1<Pipe<double[]>.PipeEndpoint>
    {

        @Override public void action(Pipe<double[]>.PipeEndpoint e)
        {
            ee1_cb(e);
        }
    }

    private void ee1_cb(Pipe<double[]>.PipeEndpoint p)
    {
        try
        {
            synchronized (ee1)
            {
                ee1.notify();
            }
        }
        catch (java.lang.Exception e)
        {}
    }

    private class ee1a_ack implements Action2<Pipe<double[]>.PipeEndpoint, Long>
    {

        @Override public void action(Pipe<double[]>.PipeEndpoint e, Long packetnum)
        {
            ee1_ack_cb(e, packetnum);
        }
    }

    private void ee1_ack_cb(Pipe<double[]>.PipeEndpoint p, long packetnum)
    {
        synchronized (this)
        {
            if (packetnum == this.packetnum)
            {
                ack_recv = true;
            }
        }
    }

    private class ee2a implements Action1<Pipe<double[]>.PipeEndpoint>
    {

        @Override public void action(Pipe<double[]>.PipeEndpoint e)
        {
            ee2_cb(e);
        }
    }

    private void ee2_cb(Pipe<double[]>.PipeEndpoint p)
    {
        try
        {
            synchronized (ee2)
            {
                ee2.notify();
            }
        }
        catch (java.lang.Exception e)
        {}
    }

    private class ee3a implements Action1<Pipe<teststruct2>.PipeEndpoint>
    {

        @Override public void action(Pipe<teststruct2>.PipeEndpoint e)
        {
            ee3_cb(e);
        }
    }

    private void ee3_cb(Pipe<teststruct2>.PipeEndpoint p)
    {
        try
        {
            synchronized (ee3)
            {
                ee3.notify();
            }
        }
        catch (java.lang.Exception e)
        {}
    }

    private boolean cb1_called;
    private boolean cb2_called;
    private boolean cb3_called;
    private boolean cb4_called;
    private boolean cb5_called;

    public final void TestCallbacks()
    {
        cb1_called = false;
        cb2_called = false;
        cb3_called = false;
        cb4_called = false;
        cb5_called = false;

        r.get_cb1().setFunction(new cb1_func());
        r.get_cb2().setFunction(new cb2_func());
        r.get_cb3().setFunction(new cb3_func());
        r.get_cb_meaning_of_life().setFunction(new cb_meaning_of_life_func());
        r.get_cb_errtest().setFunction(new cb_errtest());

        r.test_callbacks();

        RRAssert.isTrue(cb1_called);
        RRAssert.isTrue(cb2_called);
        RRAssert.isTrue(cb3_called);
        RRAssert.isTrue(cb4_called);
        RRAssert.isTrue(cb5_called);
    }

    class cb1_func implements Action
    {
        public void action()
        {
            cb1_called = true;
        }
    }

    class cb2_func implements Action2<Double, Double>
    {
        public void action(Double d1, Double d2)
        {
            RRAssert.areEqual(d1, 739.2);
            RRAssert.areEqual(d2, 0.392);
            cb2_called = true;
        }
    }

    class cb3_func implements Func2<Double, Double, Double>
    {
        public final Double func(Double d1, Double d2)
        {
            cb3_called = true;
            return d1 + d2 + 3.14;
        }
    }

    class cb_meaning_of_life_func implements Func<Integer>
    {
        public final Integer func()
        {
            cb4_called = true;
            return 42;
        }
    }

    class cb_errtest implements Action
    {
        public final void action()
        {
            cb5_called = true;
            throw new RuntimeException("This is a test");
        }
    }

    private boolean w1_called;
    private boolean w2_called;
    private boolean w3_called;

    public final void TestWires()
    {
        w1_called = false;
        w2_called = false;
        w3_called = false;

        Wire<double[]>.WireConnection w1 = r.get_w1().connect();
        Wire<teststruct2>.WireConnection w2 = r.get_w2().connect();
        Wire<MultiDimArray>.WireConnection w3 = r.get_w3().connect();

        w1.addWireValueListener(new w1_changed());
        w2.addWireValueListener(new w2_changed());
        w3.addWireValueListener(new w3_changed());
        for (int i = 0; i < 10; i++)
        {
            try
            {
                w1.setOutValue(new double[] {0});

                teststruct2 s1 = new teststruct2();
                s1.mydat = new double[] {0};
                w2.setOutValue(s1);

                MultiDimArray a1 = new MultiDimArray(new int[] {1, 1}, new int[] {0});
                w3.setOutValue(a1);

                w1.setOutValue(new double[] {-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -1.570323e+03,
                                             -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11});
                teststruct2 s2 = new teststruct2();
                s2.mydat = new double[] {-1.014645e-21, 4.743740e+11,  5.804886e-04,  2.963852e-20, 4.277621e-21,
                                         -1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10};
                w2.setOutValue(s2);

                MultiDimArray a2 = new MultiDimArray(
                    new int[] {2, 5}, new int[] {2058500854, -611248192, 197490486, -517717939, -513450368, 296469979,
                                                 645365194, 2043654604, -1672941174, 710030901});
                w3.setOutValue(a2);

                try
                {
                    java.lang.Thread.sleep(500);
                }
                catch (Exception e)
                {}

                double[] in1 = w1.getInValue();
                ca(in1, new double[] {-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -1.570323e+03,
                                      -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11});

                teststruct2 in2 = w2.getInValue();
                ca(in2.mydat, new double[] {-1.014645e-21, 4.743740e+11, 5.804886e-04, 2.963852e-20, 4.277621e-21,
                                            -1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10});

                MultiDimArray in3 = w3.getInValue();
                ca(in3.dims, new int[] {2, 5});
                ca((int[])in3.array, new int[] {2058500854, -611248192, 197490486, -517717939, -513450368, 296469979,
                                                645365194, 2043654604, -1672941174, 710030901});

                RRAssert.isTrue(w1_called);
                RRAssert.isTrue(w2_called);
                RRAssert.isTrue(w3_called);
                break;
            }
            catch (Exception e)
            {
                if (i == 9)
                {
                    throw e;
                }
                else
                {
                    try
                    {
                        java.lang.Thread.sleep(100);
                    }
                    catch (Exception e2)
                    {}
                }
            }
        }
    }

    class w1_changed implements Action3<Wire<double[]>.WireConnection, double[], TimeSpec>
    {
        public void action(Wire<double[]>.WireConnection c, double[] value, TimeSpec t)
        {
            w1_called = true;
        }
    }

    class w2_changed implements Action3<Wire<teststruct2>.WireConnection, teststruct2, TimeSpec>
    {
        public void action(Wire<teststruct2>.WireConnection c, teststruct2 value, TimeSpec t)
        {
            w2_called = true;
        }
    }

    class w3_changed implements Action3<Wire<MultiDimArray>.WireConnection, MultiDimArray, TimeSpec>
    {
        public void action(Wire<MultiDimArray>.WireConnection c, MultiDimArray value, TimeSpec t)
        {
            w3_called = true;
        }
    }

    public final void TestMemories()
    {

        test_m1();
        // test_m2();
        // test_m3();

        long l = RobotRaconteurNode.s().getMemoryMaxTransferSize();
        RobotRaconteurNode.s().setMemoryMaxTransferSize(10240);
        test_m1();
        // test_m2();
        RobotRaconteurNode.s().setMemoryMaxTransferSize(l);
    }

    private void test_m1()
    {
        RRAssert.areEqual(r.get_m1().length(), 100);
        double[] m1_1 = new double[11];
        r.get_m1().read(10, m1_1, 1, 10);
        ca(m1_1, new double[] {0, -1.478723e-16, 1.507042e-05, -2.046271e+13, 4.014775e+06, 4.140740e+10, 1.318907e+16,
                               -2.312403e+17, 4.463696e-13, 9.173421e-04, 6.169183e-21});

        double[] m1_2 =
            new double[] {3.462892e+10, -1.149841e-18, 4.649317e-15,  -3.632280e-19, -5.252280e+07, 1.800453e-07,
                          3.772468e-04, -1.911891e+09, -3.018967e-14, 4.835062e-06,  5.269663e+13,  4.946221e+03};
        r.get_m1().write(20, m1_2, 2, 8);

        double[] m1_3 = new double[100];
        r.get_m1().read(0, m1_3, 0, 100);

        double[] m1_4 = TestVectors.getDoubles("ts1.m1.initial");
        DataTypeUtil.arraycopy(m1_2, 2, m1_4, 20, 8);
        ca(m1_3, m1_4);
    }

    private void test_m2()
    {
        RRAssert.areEqual(r.get_m2().dimCount(), 5);
        long[] m2_dims = r.get_m2().dimensions();
        ca(m2_dims, new long[] {10, 10, 10, 10, 10});

        MultiDimArray m1 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray1.bin");
        MultiDimArray m2 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray2.bin");
        MultiDimArray m3 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray3.bin");
        MultiDimArray m4 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray4.bin");
        MultiDimArray m5 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray5.bin");

        // cSpell: ignore m1_dimi
        long[] m1_dimi = new long[5];
        for (int i = 0; i < 5; i++)
            m1_dimi[i] = (long)(m1.dims[i]);

        r.get_m2().write(new long[] {0, 0, 0, 0, 0}, m1, new long[] {0, 0, 0, 0, 0}, m1_dimi);

        MultiDimArray m1_2 = new MultiDimArray(new int[] {10, 10, 10, 10, 10}, new double[100000]);
        r.get_m2().read(new long[] {0, 0, 0, 0, 0}, m1_2, new long[] {0, 0, 0, 0, 0}, m1_dimi);
        ca(m1.dims, m1_2.dims);
        ca((double[])m1.array, (double[])m1_2.array);

        r.get_m2().write(new long[] {2, 2, 3, 3, 4}, m2, new long[] {0, 2, 0, 0, 0}, new long[] {1, 5, 5, 2, 1});
        MultiDimArray m2_2 = new MultiDimArray(new int[] {10, 10, 10, 10, 10}, new double[100000]);
        r.get_m2().read(new long[] {0, 0, 0, 0, 0}, m2_2, new long[] {0, 0, 0, 0, 0}, m1_dimi);

        ca(m2_2.dims, m3.dims);
        ca((double[])m2_2.array, (double[])m3.array);

        MultiDimArray m6 = new MultiDimArray(new int[] {2, 2, 1, 1, 10}, new double[40]);
        r.get_m2().read(new long[] {4, 2, 2, 8, 0}, m6, new long[] {0, 0, 0, 0, 0}, new long[] {2, 2, 1, 1, 10});
        ca(m4.dims, m6.dims);
        ca((double[])m4.array, (double[])m6.array);

        MultiDimArray m7 = new MultiDimArray(new int[] {4, 4, 4, 4, 10}, new double[2560]);
        r.get_m2().read(new long[] {4, 2, 2, 8, 0}, m7, new long[] {2, 1, 2, 1, 0}, new long[] {2, 2, 1, 1, 10});
        ca(m5.dims, m7.dims);
        ca((double[])m5.array, (double[])m7.array);
    }

    private void test_m3()
    {
        MultiDimArray m1 =
            MultiDimArrayTest.LoadByteArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray_b1.bin");
        MultiDimArray m2 =
            MultiDimArrayTest.LoadByteArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray_b2.bin");
        MultiDimArray m3 =
            MultiDimArrayTest.LoadByteArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray_b3.bin");
        MultiDimArray m4 =
            MultiDimArrayTest.LoadByteArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray_b4.bin");
        MultiDimArray m5 =
            MultiDimArrayTest.LoadByteArrayFromFile(MultiDimArrayTest.testdatapath + "/testmdarray_b5.bin");

        r.get_m3().write(new long[] {0, 0}, m1, new long[] {0, 0}, new long[] {1024, 1024});

        MultiDimArray m1_2 = new MultiDimArray(new int[] {1024, 1024}, new UnsignedBytes(1024 * 1024));
        r.get_m3().read(new long[] {0, 0}, m1_2, new long[] {0, 0}, new long[] {1024, 1024});
        ca(m1.dims, m1_2.dims);
        ca(((UnsignedBytes)m1.array).value, ((UnsignedBytes)m1_2.array).value);

        r.get_m3().write(new long[] {50, 100}, m2, new long[] {20, 25}, new long[] {200, 200});
        MultiDimArray m2_2 = new MultiDimArray(new int[] {1024, 1024}, new UnsignedBytes(1024 * 1024));
        r.get_m3().read(new long[] {0, 0}, m2_2, new long[] {0, 0}, new long[] {1024, 1024});
        ca(m3.dims, m2_2.dims);
        ca(((UnsignedBytes)m3.array).value, ((UnsignedBytes)m2_2.array).value);

        MultiDimArray m6 = new MultiDimArray(new int[] {200, 200}, new UnsignedBytes(200 * 200));
        r.get_m3().read(new long[] {65, 800}, m6, new long[] {0, 0}, new long[] {200, 200});
        ca(m4.dims, m6.dims);
        ca(((UnsignedBytes)m4.array).value, ((UnsignedBytes)m6.array).value);

        MultiDimArray m7 = new MultiDimArray(new int[] {512, 512}, new UnsignedBytes(512 * 512));
        r.get_m3().read(new long[] {65, 800}, m7, new long[] {100, 230}, new long[] {200, 200});
        ca(m5.dims, m7.dims);
        ca(((UnsignedBytes)m5.array).value, ((UnsignedBytes)m7.array).value);
    }

    public final void TestAuthentication(String url)
    {
        // Test two different logins
        java.util.HashMap<String, Object> cred1 = new java.util.HashMap<String, Object>();
        cred1.put("password", "testpass1");
        testroot r1 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser1", cred1);
        r1.func3(2.2, 3.3);
        RobotRaconteurNode.s().disconnectService(r1);

        java.util.HashMap<String, Object> cred2 = new java.util.HashMap<String, Object>();
        cred2.put("password", "testpass2");
        testroot r2 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser2", cred2);
        r2.func3(2.2, 3.3);
        RobotRaconteurNode.s().disconnectService(r2);

        // Check an invalid password
        boolean err = false;

        testroot r3 = null;
        try
        {
            r3 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser2", cred1);
            r3.func3(2.2, 3.3);
        }
        catch (RuntimeException e)
        {
            err = true;
            try
            {
                RobotRaconteurNode.s().disconnectService(r3);
            }
            catch (java.lang.Exception ee)
            {}
        }

        RRAssert.isTrue(err);
    }

    public final void TestObjectLock(String url)
    {
        // Run a test, check 6 things:
        // 1. Exclusive username lock works as expected (user with lock can access, other user can't)
        // 2. Check that session-level lock works as expected (only one session can access the locked object)
        // 3. Check that "sub-tree" objects lock as expected
        // 5. Object lock release works
        // 6. Object lock override works

        // Log in twice
        java.util.HashMap<String, Object> cred1 = new java.util.HashMap<String, Object>();
        cred1.put("password", "testpass1");
        testroot r1 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser1", cred1);

        // Log in again with "testuser1" username
        java.util.HashMap<String, Object> cred2 = new java.util.HashMap<String, Object>();
        cred2.put("password", "testpass2");
        testroot r2 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser1", cred1);

        sub1 r1_o = r1.get_o1();
        sub2 r1_o_o2 = r1_o.get_o2_1();
        sub1 r2_o = r2.get_o1();
        sub2 r2_o_o2 = r2_o.get_o2_1();

        testroot r3 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser2", cred2);
        sub1 r3_o = r3.get_o1();
        sub2 r3_o_o2 = r3_o.get_o2_1();

        // Make sure all the objects work
        r1.func3(2.2, 3.3);
        r1_o.set_d1(new double[] {1.0});
        r1_o_o2.set_data("Hello world");
        r2.func3(2.2, 3.3);
        r2_o.set_d1(new double[] {1.0});
        r2_o_o2.set_data("Hello world");
        r3.func3(2.2, 3.3);
        r3_o.set_d1(new double[] {1.0});
        r3_o_o2.set_data("Hello world");

        // Lock the object by username
        RobotRaconteurNode.s().requestObjectLock(r1_o,
                                                 RobotRaconteurObjectLockFlags.RobotRaconteurObjectLockFlags_USER_LOCK);

        // Check that all users that should access the objects can
        r1.func3(2.2, 3.3);
        r1_o.set_d1(new double[] {1.0});
        r1_o_o2.set_data("Hello world");
        r2.func3(2.2, 3.3);
        r2_o.set_d1(new double[] {1.0});
        r2_o_o2.set_data("Hello world");
        r3.func3(2.2, 3.3);

        // Check that objects that shouldn't be able to access the objects can't
        /*ShouldBeErr<RuntimeException>(delegate()
        {
            r3_o.setd1(new double[] {1.0});
        }
       );
        ShouldBeErr<RuntimeException>(delegate()
        {
            r3_o_o2.setdata("Hello world");
        }
       );*/

        // Unlock and recheck all
        RobotRaconteurNode.s().releaseObjectLock(r1_o);

        r1.func3(2.2, 3.3);
        r1_o.set_d1(new double[] {1.0});
        r1_o_o2.set_data("Hello world");
        r2.func3(2.2, 3.3);
        r2_o.set_d1(new double[] {1.0});
        r2_o_o2.set_data("Hello world");
        r3.func3(2.2, 3.3);
        r3_o.set_d1(new double[] {1.0});
        r3_o_o2.set_data("Hello world");

        // Relock, test that the lock is active, and then close the connection.  The lock should release.  The
        // second session is closed first, and should not release the lock.
        RobotRaconteurNode.s().requestObjectLock(r1_o,
                                                 RobotRaconteurObjectLockFlags.RobotRaconteurObjectLockFlags_USER_LOCK);
        /*ShouldBeErr<RuntimeException>(delegate()
        {
            r3_o.setd1(new double[] {1.0});
        }
       );*/

        r2_o.set_d1(new double[] {1.0});
        RobotRaconteurNode.s().disconnectService(r2);

        // Object still should be locked
        /*ShouldBeErr<RuntimeException>(delegate()
        {
            r3_o.setd1(new double[] {1.0});
        }
       );*/

        // Now close the session and lock should be released
        RobotRaconteurNode.s().disconnectService(r1);
        r3_o.set_d1(new double[] {1.0});

        // Reconnect the first two test sessions
        r1 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser1", cred1);
        r2 = (testroot)RobotRaconteurNode.s().connectService(url, "testuser2", cred2);
        r1_o = r1.get_o1();
        r1_o_o2 = r1_o.get_o2_1();
        r2_o = r2.get_o1();
        r2_o_o2 = r2_o.get_o2_1();

        // Test the exclusive client lock
        RobotRaconteurNode.s().requestObjectLock(
            r1_o, RobotRaconteurObjectLockFlags.RobotRaconteurObjectLockFlags_CLIENT_LOCK);
        r1_o.set_d1(new double[] {1.0});
        /*ShouldBeErr<RuntimeException>(delegate()
        {
            r2_o.setd1(new double[] {1.0});
        }
       );
        ShouldBeErr<RuntimeException>(delegate()
        {
            r3_o.setd1(new double[] {1.0});
        }
       );*/

        // Test the lock override by testsuperpass
        java.util.HashMap<String, Object> cred5 = new java.util.HashMap<String, Object>();
        cred5.put("password", "superpass1");
        testroot r5 = (testroot)RobotRaconteurNode.s().connectService(url, "testsuperuser", cred5);
        sub1 r5_o = r5.get_o1();
        RobotRaconteurNode.s().releaseObjectLock(r5_o);

        // Make sure the lock is released
        r2_o.set_d1(new double[] {1.0});
        r3_o.set_d1(new double[] {1.0});

        // Close all connections
        RobotRaconteurNode.s().disconnectService(r1);
        RobotRaconteurNode.s().disconnectService(r2);
        RobotRaconteurNode.s().disconnectService(r3);
        RobotRaconteurNode.s().disconnectService(r5);
    }

    public final void TestMonitorLock(String url)
    {

        // The monitor lock acquires an exclusive sock to a single thread on the client.
        // This lock is for a single thread, and works for all clients and the service if
        // it is checking for monitor locks.

        testroot r1 = (testroot)RobotRaconteurNode.s().connectService(url);
        testroot r2 = (testroot)RobotRaconteurNode.s().connectService(url);

        sub1 r1_o = r1.get_o1();
        sub2 r1_o_o2 = r1_o.get_o2_1();
        sub1 r2_o = r2.get_o1();
        sub2 r2_o_o2 = r2_o.get_o2_1();

        // Make sure everything works
        r1.func3(2.2, 3.3);
        r1_o.set_d1(new double[] {1.0});
        r1_o_o2.set_data("Hello world");
        r2.func3(2.2, 3.3);
        r2_o.set_d1(new double[] {1.0});
        r2_o_o2.set_data("Hello world");

        // Make sure that locks work as expected

        boolean t1 = false;
        boolean t2 = false;

        RobotRaconteurNode.s().monitorEnter(r1_o);

        t1 = true;
        /*if (!e1.WaitOne(1000))
        {
            throw new RuntimeException();
        }*/
        // Thread.sleep(10);
        r1.func3(2.2, 3.3);
        r1_o.set_d1(new double[] {0.0});
        RRAssert.isFalse(t2);
        t1 = false;
        RobotRaconteurNode.s().monitorExit(r1_o);
    }

    Object async_err_lock = new Object();
    RuntimeException async_err = null;
    CountDownLatch async_wait = new CountDownLatch(1);

    protected final void TestAsync_err(Exception exp)
    {
        synchronized (async_err_lock)
        {
            if (async_err instanceof RuntimeException)
            {
                async_err = (RuntimeException)exp;
            }
            else
            {
                async_err = new RuntimeException("");
            }
            async_wait.countDown();
        }
    }

    public final void TestAsync(String url)
    {
        java.util.HashMap<String, Object> cred1 = new java.util.HashMap<String, Object>();
        cred1.put("password", "testpass1");
        TestAsync1 h = new TestAsync1();
        RobotRaconteurNode.s().asyncConnectService(url, "testuser1", cred1, null, null, h);

        try
        {

            async_wait.await();
        }
        catch (InterruptedException e)
        {}

        synchronized (async_err_lock)
        {
            if (async_err != null)
                throw async_err;
        }
    }

    protected class TestAsync1 implements Action2<Object, RuntimeException>
    {

        @Override public void action(Object p1, RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                async_testroot r1 = (async_testroot)p1;
                TestAsync2 h = new TestAsync2(r1);
                r1.async_get_d2(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync2 implements Action2<double[], RuntimeException>
    {

        async_testroot r;

        public TestAsync2(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(double[] ret, RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {

                ca(ret, TestVectors.getDoubles("ts1.d2.to_client"));
                double[] d2_dat = TestVectors.getDoubles("ts1.d2.to_service");

                TestAsync3 h = new TestAsync3(r);
                r.async_set_d2(d2_dat, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync3 implements Action1<RuntimeException>
    {

        async_testroot r;

        public TestAsync3(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync4 h = new TestAsync4(r);
                r.async_func1(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync4 implements Action1<RuntimeException>
    {

        async_testroot r;

        public TestAsync4(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync5 h = new TestAsync5(r);
                r.async_func3(2, 3.45, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync5 implements Action2<Double, RuntimeException>
    {

        async_testroot r;

        public TestAsync5(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(Double ret, RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                if (ret != 5.45)
                    throw new Exception("");
                TestAsync6 h = new TestAsync6(r);
                r.async_func_errtest(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync6 implements Action1<RuntimeException>
    {

        async_testroot r;

        public TestAsync6(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(RuntimeException p2)
        {
            if (p2 == null)
            {
                TestAsync_err(new RuntimeException(""));
                return;
            }

            try
            {
                TestAsync7 h = new TestAsync7(r);
                r.async_get_o1(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync7 implements Action2<sub1, RuntimeException>
    {

        async_testroot r;

        public TestAsync7(async_testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(sub1 o1, RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                if (o1 == null)
                    throw new RuntimeException("");
                double[] d1 = o1.get_d1();

                testroot r1 = (testroot)r;
                TestAsync8 h = new TestAsync8(r1);
                r1.get_p1().asyncConnect(-1, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync8 implements Action2<Pipe<double[]>.PipeEndpoint, RuntimeException>
    {
        testroot r;

        public TestAsync8(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(Pipe<double[]>.PipeEndpoint p1, RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync9 h = new TestAsync9(r, p1);

                p1.asyncSendPacket(new double[] {1, 2, 3, 4}, h);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync9 implements Action2<Long, RuntimeException>
    {
        testroot r;
        Pipe<double[]>.PipeEndpoint e1;

        public TestAsync9(testroot r, Pipe<double[]>.PipeEndpoint e1)
        {
            this.r = r;
            this.e1 = e1;
        }

        @Override public void action(Long p1, RuntimeException p2)
        {
            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync10 h = new TestAsync10(r);

                e1.asyncClose(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync10 implements Action1<RuntimeException>
    {
        testroot r;

        public TestAsync10(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync11 h = new TestAsync11(r);
                r.get_w1().asyncConnect(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync11 implements Action2<Wire<double[]>.WireConnection, RuntimeException>
    {
        testroot r;

        public TestAsync11(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(Wire<double[]>.WireConnection p1, RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                p1.setOutValue(new double[] {0.0});
                TestAsync12 h = new TestAsync12(r);

                p1.asyncClose(h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync12 implements Action1<RuntimeException>
    {
        testroot r;

        public TestAsync12(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                TestAsync13 h = new TestAsync13(r);
                RobotRaconteurNode.s().asyncRequestObjectLock(
                    r, RobotRaconteurObjectLockFlags.RobotRaconteurObjectLockFlags_CLIENT_LOCK, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync13 implements Action2<String, RuntimeException>
    {
        testroot r;

        public TestAsync13(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(String ret, RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                if (!ret.equals("OK"))
                    throw new Exception("");
                TestAsync14 h = new TestAsync14(r);
                RobotRaconteurNode.s().asyncReleaseObjectLock(r, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync14 implements Action2<String, RuntimeException>
    {
        testroot r;

        public TestAsync14(testroot r1)
        {
            this.r = r1;
        }

        @Override public void action(String ret, RuntimeException p2)
        {

            if (p2 != null)
            {
                TestAsync_err(p2);
                return;
            }

            try
            {
                if (!ret.equals("OK"))
                    throw new Exception("");
                TestAsync15 h = new TestAsync15();
                RobotRaconteurNode.s().asyncDisconnectService(r, h, 5000);
            }
            catch (Exception e)
            {
                TestAsync_err(e);
            }
        }
    }

    protected class TestAsync15 implements Action
    {

        public void action()
        {
            async_wait.countDown();
        }
    }
}

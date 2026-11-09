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
        r.set_d3(TestVectors.getDoubles("ts1.d3.to_service"));
        ca(r.get_d3(), TestVectors.getDoubles("ts1.d3.to_client"));
        r.set_d4(TestVectors.getDoubles("ts1.d4.to_service"));
        ca(r.get_d4(), TestVectors.getDoubles("ts1.d4.to_client"));

        MultiDimArray d5_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.d5.to_service.dims"),
                                               TestVectors.getDoubles("ts1.d5.to_service"));

        r.set_d5(d5_1);

        MultiDimArray d5_2 = r.get_d5();
        ca(d5_2.dims, TestVectors.getUInt32s("ts1.d5.to_client.dims"));
        ca((double[])d5_2.array, TestVectors.getDoubles("ts1.d5.to_client"));

        MultiDimArray d6_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.d6.to_service.dims"),
                                               TestVectors.getDoubles("ts1.d6.to_service"));

        r.set_d6(d6_1);

        MultiDimArray d6_2 = r.get_d6();
        ca(d6_2.dims, TestVectors.getUInt32s("ts1.d6.to_client.dims"));
        ca((double[])d6_2.array, TestVectors.getDoubles("ts1.d6.to_client"));

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

        MultiDimArray u8_3_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.u8_3.to_service.dims"),
                                                 new UnsignedBytes(TestVectors.getUInt8s("ts1.u8_3.to_service")));
        r.set_u8_3(u8_3_1);
        MultiDimArray u8_3_2 = r.get_u8_3();
        ca(TestVectors.getUInt32s("ts1.u8_3.to_client.dims"), u8_3_2.dims);
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
        ca(s1.dstruct2.get("anothertest").mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_3.to_client"));

        RRAssert.areEqual(s1.lstruct3.size(), 2);
        ca(s1.lstruct3.get(0).mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client"));
        ca(s1.lstruct3.get(1).mydat, TestVectors.getDoubles("ts1.struct1.dstruct2_3.to_client"));

        ca(s1.multidimarray.dims, TestVectors.getUInt32s("ts1.struct1.multidimarray.to_client.dims"));
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

        s2.multidimarray = new MultiDimArray(TestVectors.getUInt32s("ts1.struct1.multidimarray.to_service.dims"),
                                             TestVectors.getDoubles("ts1.struct1.multidimarray.to_service"));
        s2.var3 = TestVectors.getDoubles("ts1.struct1.var3.to_service");
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
        ca(is_d3_1.get(12), TestVectors.getDoubles("ts1.is_d3.to_client.a"));
        ca(is_d3_1.get(832), TestVectors.getDoubles("ts1.is_d3.to_client.b"));

        java.util.HashMap<Integer, double[]> is_d3_2 = new java.util.HashMap<Integer, double[]>();
        is_d3_2.put(47, TestVectors.getDoubles("ts1.is_d3.to_service.a"));
        is_d3_2.put(324, TestVectors.getDoubles("ts1.is_d3.to_service.b"));
        r.set_is_d3(is_d3_2);

        // Test is_d4

        java.util.Map<String, double[]> is_d4_1 = r.get_is_d4();
        RRAssert.areEqual(is_d4_1.size(), 2);
        ca(is_d4_1.get("testval1"), TestVectors.getDoubles("ts1.is_d4.to_client.a"));
        ca(is_d4_1.get("testval2"), TestVectors.getDoubles("ts1.is_d4.to_client.b"));

        java.util.HashMap<String, double[]> is_d4_2 = new java.util.HashMap<String, double[]>();
        is_d4_2.put("testval3", TestVectors.getDoubles("ts1.is_d4.to_service.a"));
        is_d4_2.put("testval4", TestVectors.getDoubles("ts1.is_d4.to_service.b"));
        r.set_is_d4(is_d4_2);

        // Test is_d5
        java.util.Map<Integer, MultiDimArray> is_d5_1 = r.get_is_d5();
        RRAssert.areEqual(is_d5_1.size(), 1);
        MultiDimArray is_d5_1_1 = is_d5_1.get(564);
        ca(is_d5_1_1.dims, TestVectors.getUInt32s("ts1.is_d5.to_client.dims"));
        ca((double[])is_d5_1_1.array, TestVectors.getDoubles("ts1.is_d5.to_client"));

        MultiDimArray is_d5_2_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d5.to_service.dims"),
                                                    TestVectors.getDoubles("ts1.is_d5.to_service"));
        java.util.HashMap<Integer, MultiDimArray> is_d5_2 = new java.util.HashMap<Integer, MultiDimArray>();
        is_d5_2.put(328, is_d5_2_1);

        // Test is_d6
        java.util.Map<String, MultiDimArray> is_d6_1 = r.get_is_d6();
        RRAssert.areEqual(is_d6_1.size(), 1);
        MultiDimArray is_d6_1_1 = is_d6_1.get("testval1");
        ca(is_d6_1_1.dims, TestVectors.getUInt32s("ts1.is_d6.to_client.dims"));
        ca((double[])is_d6_1_1.array, TestVectors.getDoubles("ts1.is_d6.to_client"));

        MultiDimArray is_d6_2_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d6.to_service.dims"),
                                                    TestVectors.getDoubles("ts1.is_d6.to_service"));
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
        ca(is_struct1_1.get(748).mydat, TestVectors.getDoubles("ts1.is_struct1.to_client"));

        java.util.HashMap<Integer, teststruct2> is_struct1_2 = new java.util.HashMap<Integer, teststruct2>();
        teststruct2 is_struct1_2_1 = new teststruct2();
        is_struct1_2_1.mydat = TestVectors.getDoubles("ts1.is_struct1.to_service");
        is_struct1_2.put(372, is_struct1_2_1);
        r.set_is_struct1(is_struct1_2);

        // Test is_struct2
        java.util.Map<String, teststruct2> is_struct2_1 = r.get_is_struct2();
        ca(is_struct2_1.get("testval1").mydat, TestVectors.getDoubles("ts1.is_struct2.to_client"));

        java.util.HashMap<String, teststruct2> is_struct2_2 = new java.util.HashMap<String, teststruct2>();
        teststruct2 is_struct2_2_1 = new teststruct2();
        is_struct2_2_1.mydat = TestVectors.getDoubles("ts1.is_struct2.to_service");
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
        ca(list_d3_1.get(0), TestVectors.getDoubles("ts1.is_d3.to_client.a"));
        ca(list_d3_1.get(1), TestVectors.getDoubles("ts1.is_d3.to_client.b"));

        java.util.List<double[]> list_d3_2 = new java.util.ArrayList<double[]>();
        list_d3_2.add(TestVectors.getDoubles("ts1.is_d3.to_service.a"));
        list_d3_2.add(TestVectors.getDoubles("ts1.is_d3.to_service.b"));
        r.set_list_d3(list_d3_2);

        // Test list_d5
        java.util.List<MultiDimArray> list_d5_1 = r.get_list_d5();
        RRAssert.areEqual(list_d5_1.size(), 1);
        MultiDimArray list_d5_1_1 = list_d5_1.get(0);
        ca(list_d5_1_1.dims, TestVectors.getUInt32s("ts1.is_d5.to_client.dims"));
        ca((double[])list_d5_1_1.array, TestVectors.getDoubles("ts1.is_d5.to_client"));

        MultiDimArray list_d5_2_1 = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d5.to_service.dims"),
                                                      TestVectors.getDoubles("ts1.is_d5.to_service"));
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
        ca(list_struct1_1.get(0).mydat, TestVectors.getDoubles("ts1.is_struct1.to_client"));

        java.util.List<teststruct2> list_struct1_2 = new java.util.ArrayList<teststruct2>();
        teststruct2 list_struct1_2_1 = new teststruct2();
        list_struct1_2_1.mydat = TestVectors.getDoubles("ts1.is_struct1.to_service");
        list_struct1_2.add(is_struct1_2_1);
        r.set_list_struct1(list_struct1_2);

        // Test struct3
        ca(r.get_struct3().a1, TestVectors.getDoubles("ts1.struct3.to_client"));

        ostruct2 struct3_1 = new ostruct2();
        struct3_1.a1 = TestVectors.getDoubles("ts1.struct3.to_service");
        r.set_struct3(struct3_1);

        // Test var1  TODO...

        // Test var2 TODO...

        // Test var_num
        ca((int[])r.get_var_num(), TestVectors.getInts("ts1.var_num.to_client"));
        r.set_var_num(TestVectors.getInts("ts1.var_num.to_service"));

        // Test var_str
        RRAssert.areEqual(((String)r.get_var_str()), "Hello Client!");
        r.set_var_str("Hello Server!");

        // Test var_struct
        ca(((teststruct2)r.get_var_struct()).mydat, TestVectors.getDoubles("ts1.var_struct.to_client"));

        teststruct2 var_struct_1 = new teststruct2();
        var_struct_1.mydat = TestVectors.getDoubles("ts1.var_struct.to_service");
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
        ca(var_a_1.dims, TestVectors.getUInt32s("ts1.var_multidimarray.to_client.dims"));
        ca((double[])var_a_1.array, TestVectors.getDoubles("ts1.var_multidimarray.to_client"));
        r.set_var_multidimarray(new MultiDimArray(TestVectors.getUInt32s("ts1.var_multidimarray.to_service.dims"),
                                                  TestVectors.getDoubles("ts1.var_multidimarray.to_service")));

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

        o1.set_d1(TestVectors.getDoubles("ts1.client.o_d1a"));
        o2_10.set_d1(TestVectors.getDoubles("ts1.client.o_d1b"));
        o2_34.set_d1(TestVectors.getDoubles("ts1.client.o_d1c"));
        o3_1.set_d1(TestVectors.getDoubles("ts1.client.o_d1d"));
        o4_myind.set_d1(TestVectors.getDoubles("ts1.client.o_d1e"));
        o4_specialind.set_d1(TestVectors.getDoubles("ts1.client.o_d1f"));
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
        e1.sendPacket(TestVectors.getDoubles("ts1.client.d3"));

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
        ca(e1.receivePacketWait(1000), TestVectors.getDoubles("ts1.client.d3"));

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

                w1.setOutValue(TestVectors.getDoubles("ts1.client.d1"));
                teststruct2 s2 = new teststruct2();
                s2.mydat = TestVectors.getDoubles("ts1.client.d2");
                w2.setOutValue(s2);

                MultiDimArray a2 = new MultiDimArray(TestVectors.getUInt32s("ts1.client.i3.dims"),
                                                     TestVectors.getInts("ts1.client.i3"));
                w3.setOutValue(a2);

                try
                {
                    java.lang.Thread.sleep(500);
                }
                catch (Exception e)
                {}

                double[] in1 = w1.getInValue();
                ca(in1, TestVectors.getDoubles("ts1.client.d1"));

                teststruct2 in2 = w2.getInValue();
                ca(in2.mydat, TestVectors.getDoubles("ts1.client.d2"));

                MultiDimArray in3 = w3.getInValue();
                ca(in3.dims, TestVectors.getUInt32s("ts1.client.i3.dims"));
                ca((int[])in3.array, TestVectors.getInts("ts1.client.i3"));

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
        ca(m1_1, TestVectors.getDoubles("ts1.client.m1_1a"));

        double[] m1_2 = TestVectors.getDoubles("ts1.client.m1_2a");
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

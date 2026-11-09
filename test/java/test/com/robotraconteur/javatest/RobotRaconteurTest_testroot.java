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
        return TestVectors.getDoubles("ts1.d3.to_client");
    }
    public final void set_d3(double[] value)
    {
        double[] testval = TestVectors.getDoubles("ts1.d3.to_service");
        ca(testval, value);
    }

    public final double[] get_d4()
    {
        return TestVectors.getDoubles("ts1.d4.to_client");
    }
    public final void set_d4(double[] value)
    {
        double[] testval = TestVectors.getDoubles("ts1.d4.to_service");
        ca(value, testval);
    }

    public final MultiDimArray get_d5()
    {
        double[] r = TestVectors.getDoubles("ts1.d5.to_client");

        int[] dims = TestVectors.getUInt32s("ts1.d5.to_client.dims");
        return new MultiDimArray(dims, r);
    }
    public final void set_d5(MultiDimArray value)
    {
        double[] r = TestVectors.getDoubles("ts1.d5.to_service");

        int[] dims = TestVectors.getUInt32s("ts1.d5.to_service.dims");

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
        int[] dims = TestVectors.getUInt32s("ts1.u8_3.to_client.dims");
        return new MultiDimArray(dims, new UnsignedBytes(r));
    }
    public final void set_u8_3(MultiDimArray value)
    {
        byte[] testval = TestVectors.getUInt8s("ts1.u8_3.to_service");
        int[] testdims = TestVectors.getUInt32s("ts1.u8_3.to_service.dims");
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
        d2_2.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_3.to_client");
        d2.put("anothertest", d2_2);

        s.dstruct2 = d2;

        java.util.List<teststruct2> l3 = new java.util.ArrayList<teststruct2>();
        teststruct2 l3_1 = new teststruct2();
        l3_1.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_1.to_client");
        l3.add(l3_1);
        teststruct2 l3_2 = new teststruct2();
        l3_2.mydat = TestVectors.getDoubles("ts1.struct1.dstruct2_3.to_client");
        l3.add(l3_2);
        s.lstruct3 = l3;

        double[] md = TestVectors.getDoubles("ts1.struct1.multidimarray.to_client");
        s.multidimarray = new MultiDimArray(TestVectors.getUInt32s("ts1.struct1.multidimarray.to_client.dims"), md);
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

        ca(value.multidimarray.dims, TestVectors.getUInt32s("ts1.struct1.multidimarray.to_service.dims"));
        ca((double[])value.multidimarray.array, TestVectors.getDoubles("ts1.struct1.multidimarray.to_service"));

        ca((double[])value.var3, TestVectors.getDoubles("ts1.struct1.var3.to_service"));
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
        o.put(12, TestVectors.getDoubles("ts1.is_d3.to_client.a"));
        o.put(832, TestVectors.getDoubles("ts1.is_d3.to_client.b"));
        return o;
    }
    public final void set_is_d3(java.util.Map<Integer, double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get(47), TestVectors.getDoubles("ts1.is_d3.to_service.a"));
        ca(value.get(324), TestVectors.getDoubles("ts1.is_d3.to_service.b"));
    }

    public final java.util.Map<String, double[]> get_is_d4()
    {
        java.util.HashMap<String, double[]> o = new java.util.HashMap<String, double[]>();
        o.put("testval1", TestVectors.getDoubles("ts1.is_d4.to_client.a"));
        o.put("testval2", TestVectors.getDoubles("ts1.is_d4.to_client.b"));
        return o;
    }
    public final void set_is_d4(java.util.Map<String, double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get("testval3"), TestVectors.getDoubles("ts1.is_d4.to_service.a"));
        ca(value.get("testval4"), TestVectors.getDoubles("ts1.is_d4.to_service.b"));
    }

    public final java.util.Map<Integer, MultiDimArray> get_is_d5()
    {
        MultiDimArray a = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d5.to_client.dims"),
                                            TestVectors.getDoubles("ts1.is_d5.to_client"));
        java.util.HashMap<Integer, MultiDimArray> o = new java.util.HashMap<Integer, MultiDimArray>();
        o.put(564, a);
        return o;
    }
    public final void set_is_d5(java.util.Map<Integer, MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get(328);
        ca(a.dims, TestVectors.getUInt32s("ts1.is_d5.to_service.dims"));
        ca((double[])a.array, TestVectors.getDoubles("ts1.is_d5.to_service"));
    }

    public final java.util.Map<String, MultiDimArray> get_is_d6()
    {
        MultiDimArray a = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d6.to_client.dims"),
                                            TestVectors.getDoubles("ts1.is_d6.to_client"));
        java.util.HashMap<String, MultiDimArray> o = new java.util.HashMap<String, MultiDimArray>();
        o.put("testval1", a);
        return o;
    }
    public final void set_is_d6(java.util.Map<String, MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get("testval2");
        ca(a.dims, TestVectors.getUInt32s("ts1.is_d6.to_service.dims"));
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
        s.mydat = TestVectors.getDoubles("ts1.is_struct1.to_client");
        o.put(748, s);
        return o;
    }
    public final void set_is_struct1(java.util.Map<Integer, teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get(372);
        ca(s.mydat, TestVectors.getDoubles("ts1.is_struct1.to_service"));
    }

    public final java.util.Map<String, teststruct2> get_is_struct2()
    {
        java.util.HashMap<String, teststruct2> o = new java.util.HashMap<String, teststruct2>();
        teststruct2 s = new teststruct2();
        s.mydat = TestVectors.getDoubles("ts1.is_struct2.to_client");
        o.put("testval1", s);
        return o;
    }
    public final void set_is_struct2(java.util.Map<String, teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get("testval2");
        ca(s.mydat, TestVectors.getDoubles("ts1.is_struct2.to_service"));
    }

    public final ostruct2 get_struct3()
    {
        ostruct2 o = new ostruct2();
        o.a1 = TestVectors.getDoubles("ts1.struct3.to_client");
        return o;
    }
    public final void set_struct3(ostruct2 value)
    {
        ca(value.a1, TestVectors.getDoubles("ts1.struct3.to_service"));
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
        return TestVectors.getInts("ts1.var_num.to_client");
    }
    public final void set_var_num(Object value)
    {
        ca((int[])value, TestVectors.getInts("ts1.var_num.to_service"));
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
        s.mydat = TestVectors.getDoubles("ts1.var_struct.to_client");
        return s;
    }
    public final void set_var_struct(Object value)
    {
        ca(((teststruct2)value).mydat, TestVectors.getDoubles("ts1.var_struct.to_service"));
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
        double[] d = TestVectors.getDoubles("ts1.var_multidimarray.to_client");
        return new MultiDimArray(TestVectors.getUInt32s("ts1.var_multidimarray.to_client.dims"), d);
    }
    public final void set_var_multidimarray(Object value)
    {
        MultiDimArray a = (MultiDimArray)value;
        ca(a.dims, TestVectors.getUInt32s("ts1.var_multidimarray.to_service.dims"));
        double[] d = TestVectors.getDoubles("ts1.var_multidimarray.to_service");
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
        double[] r = TestVectors.getDoubles("ts1.d6.to_client");

        int[] dims = TestVectors.getUInt32s("ts1.d6.to_client.dims");
        return new MultiDimArray(dims, r);
    }
    @Override public void set_d6(MultiDimArray value)
    {
        double[] r = TestVectors.getDoubles("ts1.d6.to_service");

        int[] dims = TestVectors.getUInt32s("ts1.d6.to_service.dims");

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
        o.add(TestVectors.getDoubles("ts1.is_d3.to_client.a"));
        o.add(TestVectors.getDoubles("ts1.is_d3.to_client.b"));
        return o;
    }
    @Override public void set_list_d3(List<double[]> value)
    {
        RRAssert.areEqual(value.size(), 2);
        ca(value.get(0), TestVectors.getDoubles("ts1.is_d3.to_service.a"));
        ca(value.get(1), TestVectors.getDoubles("ts1.is_d3.to_service.b"));
    }
    @Override public List<MultiDimArray> get_list_d5()
    {
        MultiDimArray a = new MultiDimArray(TestVectors.getUInt32s("ts1.is_d5.to_client.dims"),
                                            TestVectors.getDoubles("ts1.is_d5.to_client"));
        java.util.ArrayList<MultiDimArray> o = new java.util.ArrayList<MultiDimArray>();
        o.add(a);
        return o;
    }
    @Override public void set_list_d5(List<MultiDimArray> value)
    {
        RRAssert.areEqual(value.size(), 1);
        MultiDimArray a = value.get(0);
        ca(a.dims, TestVectors.getUInt32s("ts1.is_d5.to_service.dims"));
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
        s.mydat = TestVectors.getDoubles("ts1.is_struct1.to_client");
        o.add(s);
        return o;
    }
    @Override public void set_list_struct1(List<teststruct2> value)
    {
        RRAssert.areEqual(value.size(), 1);
        teststruct2 s = value.get(0);
        ca(s.mydat, TestVectors.getDoubles("ts1.is_struct1.to_service"));
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

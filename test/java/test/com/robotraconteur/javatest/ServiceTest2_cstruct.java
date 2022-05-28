package com.robotraconteur.javatest;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.robotraconteur.NamedMultiDimArray;
import com.robotraconteur.PodMultiDimArray;
import com.robotraconteur.testing.TestService3.*;

class ServiceTest2_pod
{
    private static double[] create_double_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        double[] o = new double[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_double();
        }
        return o;
    }

    private static void verify_double_array(ServiceTest2_test_sequence_gen gen, double[] v, int len)
    {
        RRAssert.areEqual(v.length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.areEqual(v[i], gen.get_double());
        }
    }

    private static long[] create_uint32_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        long[] o = new long[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_uint32();
        }
        return o;
    }

    private static void verify_uint32_array(ServiceTest2_test_sequence_gen gen, long[] v, int len)
    {
        RRAssert.areEqual(v.length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.areEqual(v[i], gen.get_uint32());
        }
    }

    private static byte[] create_int8_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        byte[] o = new byte[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_int8();
        }
        return o;
    }

    private static void verify_int8_array(ServiceTest2_test_sequence_gen gen, byte[] v, int len)
    {
        RRAssert.areEqual(v.length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.areEqual(v[i], gen.get_int8());
        }
    }

    public static void fill_testpod1(testpod1 s, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        s.d1 = gen.get_double();
        s.d2 = create_double_array(gen, 6);
        s.d3 = create_double_array(gen, (int)(gen.get_uint32() % 6));
        s.d4 = create_double_array(gen, 9);

        s.s1 = new testpod2();
        fill_testpod2(s.s1, gen.get_uint32());
        s.s2 = create_testpod2_array(gen, 8);
        s.s3 = create_testpod2_array(gen, (int)(gen.get_uint32() % 9));
        s.s4 = create_testpod2_array(gen, 8);

        s.t1 = new transform();
        fill_transform(s.t1, gen.get_uint32());

        s.t2 = new transform[4];
        for (int i = 0; i < 4; i++)
        {
            s.t2[i] = new transform();
            fill_transform(s.t2[i], gen.get_uint32());
        }

        int t3_len = (int)(gen.get_uint32() % 15);
        s.t3 = new transform[(t3_len)];
        for (int i = 0; i < t3_len; i++)
        {
            s.t3[i] = new transform();
            fill_transform(s.t3[i], gen.get_uint32());
        }

        s.t4 = new transform[8];
        for (int i = 0; i < 8; i++)
        {
            s.t4[i] = new transform();
            fill_transform(s.t4[i], gen.get_uint32());
        }
    }

    public static void verify_testpod1(testpod1 s, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.areEqual(s.d1, gen.get_double());
        verify_double_array(gen, s.d2, 6);
        verify_double_array(gen, s.d3, (int)(gen.get_uint32() % 6));

        verify_double_array(gen, (double[])s.d4, 9);
        verify_testpod2(s.s1, gen.get_uint32());
        verify_testpod2_array(gen, s.s2, 8);
        verify_testpod2_array(gen, s.s3, (int)(gen.get_uint32() % 9));
        verify_testpod2_array(gen, (testpod2[])s.s4, 8);

        verify_transform(s.t1, gen.get_uint32());

        for (int i = 0; i < 4; i++)
            verify_transform(s.t2[i], gen.get_uint32());

        int t3_len = (int)(gen.get_uint32() % 15);
        RRAssert.areEqual(s.t3.length, t3_len);
        for (int i = 0; i < t3_len; i++)
            verify_transform(s.t3[i], gen.get_uint32());

        for (int i = 0; i < 8; i++)
            verify_transform(s.t4[i], gen.get_uint32());
    }

    public static void fill_testpod2(testpod2 s, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        s.i1 = gen.get_int8();
        s.i2 = create_int8_array(gen, 15);
        s.i3 = create_int8_array(gen, (int)(gen.get_uint32() % 15));
    }

    public static void verify_testpod2(testpod2 s, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.areEqual(s.i1, gen.get_int8());
        verify_int8_array(gen, s.i2, 15);
        verify_int8_array(gen, s.i3, (int)(gen.get_uint32() % 15));
    }

    public static testpod1[] create_testpod1_array(int len, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        testpod1[] o = new testpod1[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = new testpod1();
            fill_testpod1(o[i], gen.get_uint32());
        }
        return o;
    }

    public static void verify_testpod1_array(testpod1[] v, int len, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.areEqual(v.length, len);
        for (int i = 0; i < len; i++)
        {
            verify_testpod1(v[i], gen.get_uint32());
        }
    }

    public static PodMultiDimArray create_testpod1_multidimarray(int m, int n, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        testpod1[] o = new testpod1[m * n];
        for (int i = 0; i < m * n; i++)
        {
            o[i] = new testpod1();
            fill_testpod1(o[i], gen.get_uint32());
        }
        return new PodMultiDimArray(new int[] {m, n}, o);
    }

    public static void verify_testpod1_multidimarray(PodMultiDimArray v, int m, int n, long seed)
    {
        RRAssert.isTrue(Arrays.equals(v.dims, new int[] {m, n}));
        verify_testpod1_array((testpod1[])v.pod_array, m * n, seed);
    }

    public static void verify_testpod1_multidimarray(Object v, int m, int n, long seed)
    {
        if (!(v instanceof PodMultiDimArray) && n == 1)
        {
            verify_testpod1_array((testpod1[])v, m, seed);
        }
        else
        {
            PodMultiDimArray v2 = (PodMultiDimArray)v;
            RRAssert.isTrue(Arrays.equals(v2.dims, new int[] {m, n}));
            verify_testpod1_array((testpod1[])v2.pod_array, m * n, seed);
        }
    }

    public static testpod2[] create_testpod2_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        testpod2[] o = new testpod2[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = new testpod2();
            fill_testpod2(o[i], gen.get_uint32());
        }
        return o;
    }

    public static void verify_testpod2_array(ServiceTest2_test_sequence_gen gen, testpod2[] v, int len)
    {
        RRAssert.areEqual(v.length, len);
        for (int i = 0; i < len; i++)
        {
            verify_testpod2(v[i], gen.get_uint32());
        }
    }

    public static teststruct3 fill_teststruct3(long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        teststruct3 o = new teststruct3();
        o.s1 = new testpod1();
        fill_testpod1(o.s1, gen.get_uint32());
        long s2_seed = gen.get_uint32();
        o.s2 = create_testpod1_array((int)(s2_seed % 17), s2_seed);
        o.s3 = create_testpod1_array(11, gen.get_uint32());
        long s4_seed = gen.get_uint32();
        o.s4 = create_testpod1_array((int)(s4_seed % 16), s4_seed);
        o.s5 = create_testpod1_multidimarray(3, 3, gen.get_uint32());
        long s6_seed = gen.get_uint32();
        o.s6 = create_testpod1_multidimarray((int)(s6_seed % 6), (int)(s6_seed % 3), s6_seed);
        o.s7 = new ArrayList<testpod1[]>();
        testpod1 s7_1 = new testpod1();
        fill_testpod1(s7_1, gen.get_uint32());
        o.s7.add(new testpod1[] {s7_1});

        o.s8 = new ArrayList<testpod1[]>();
        o.s8.add(create_testpod1_array(2, gen.get_uint32()));
        o.s8.add(create_testpod1_array(4, gen.get_uint32()));

        o.s9 = new ArrayList<PodMultiDimArray>();
        o.s9.add(create_testpod1_multidimarray(2, 3, gen.get_uint32()));
        o.s9.add(create_testpod1_multidimarray(4, 5, gen.get_uint32()));

        testpod1 s10 = new testpod1();
        fill_testpod1(s10, gen.get_uint32());
        o.s10 = new testpod1[] {s10};

        o.s11 = create_testpod1_array(3, gen.get_uint32());
        o.s12 = create_testpod1_multidimarray(2, 2, gen.get_uint32());

        testpod1 s13 = new testpod1();
        fill_testpod1(s13, gen.get_uint32());
        List<Object> s13_1 = new ArrayList<Object>();
        ;
        s13_1.add(new testpod1[] {s13});
        o.s13 = s13_1;

        List<Object> s14 = new ArrayList<Object>();
        s14.add(create_testpod1_array(3, gen.get_uint32()));
        s14.add(create_testpod1_array(5, gen.get_uint32()));
        o.s14 = s14;

        List<Object> s15 = new ArrayList<Object>();
        s15.add(create_testpod1_multidimarray(7, 2, gen.get_uint32()));
        s15.add(create_testpod1_multidimarray(5, 1, gen.get_uint32()));
        o.s15 = s15;

        o.t1 = new transform();
        fill_transform(o.t1, gen.get_uint32());

        o.t2 = fill_transform_array(4, gen.get_uint32());
        o.t3 = fill_transform_multidimarray(2, 4, gen.get_uint32());

        o.t4 = fill_transform_array(10, gen.get_uint32());
        o.t5 = fill_transform_multidimarray(6, 5, gen.get_uint32());

        o.t6 = new ArrayList<transform[]>();
        transform t6_1 = new transform();
        fill_transform(t6_1, gen.get_uint32());
        o.t6.add(new transform[] {t6_1});

        o.t7 = new ArrayList<transform[]>();
        o.t7.add(fill_transform_array(4, gen.get_uint32()));
        o.t7.add(fill_transform_array(4, gen.get_uint32()));

        o.t8 = new ArrayList<NamedMultiDimArray>();
        o.t8.add(fill_transform_multidimarray(2, 4, gen.get_uint32()));
        o.t8.add(fill_transform_multidimarray(2, 4, gen.get_uint32()));

        transform t9 = new transform();
        fill_transform(t9, gen.get_uint32());
        ArrayList<Object> t9_1 = new ArrayList<Object>();
        t9_1.add(new transform[] {t9});
        o.t9 = t9_1;

        ArrayList<Object> t10 = new ArrayList<Object>();
        t10.add(fill_transform_array(3, gen.get_uint32()));
        t10.add(fill_transform_array(5, gen.get_uint32()));
        o.t10 = t10;

        ArrayList<Object> t11 = new ArrayList<Object>();
        t11.add(fill_transform_multidimarray(7, 2, gen.get_uint32()));
        t11.add(fill_transform_multidimarray(5, 1, gen.get_uint32()));
        o.t11 = t11;

        return o;
    }

    public static void verify_teststruct3(teststruct3 v, long seed)
    {
        RRAssert.areNotEqual(v, null);
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);

        verify_testpod1(v.s1, gen.get_uint32());
        long s2_seed = gen.get_uint32();
        verify_testpod1_array(v.s2, (int)(s2_seed % 17), s2_seed);
        verify_testpod1_array(v.s3, 11, gen.get_uint32());
        long s4_seed = gen.get_uint32();
        verify_testpod1_array(v.s4, (int)(s4_seed % 16), s4_seed);
        verify_testpod1_multidimarray(v.s5, 3, 3, gen.get_uint32());
        long s6_seed = gen.get_uint32();
        verify_testpod1_multidimarray(v.s6, (int)(s6_seed % 6), (int)(s6_seed % 3), s6_seed);

        RRAssert.areNotEqual(v.s7, null);
        RRAssert.areEqual(v.s7.size(), 1);
        testpod1 s7_0 = v.s7.get(0)[0];
        verify_testpod1(s7_0, gen.get_uint32());

        RRAssert.areNotEqual(v.s8, null);
        RRAssert.areEqual(v.s8.size(), 2);
        verify_testpod1_array(v.s8.get(0), 2, gen.get_uint32());
        verify_testpod1_array(v.s8.get(1), 4, gen.get_uint32());

        RRAssert.areNotEqual(v.s9, null);
        RRAssert.areEqual(v.s9.size(), 2);
        verify_testpod1_multidimarray(v.s9.get(0), 2, 3, gen.get_uint32());
        verify_testpod1_multidimarray(v.s9.get(1), 4, 5, gen.get_uint32());

        testpod1 s10 = ((testpod1[])v.s10)[0];
        verify_testpod1(s10, gen.get_uint32());

        verify_testpod1_array((testpod1[])v.s11, 3, gen.get_uint32());
        verify_testpod1_multidimarray(v.s12, 2, 2, gen.get_uint32());

        RRAssert.areNotEqual(v.s13, null);
        Object s13_1 = ((List<Object>)v.s13).get(0);
        testpod1[] s13 = (testpod1[])((List<Object>)v.s13).get(0);
        verify_testpod1(s13[0], gen.get_uint32());

        RRAssert.areNotEqual(v.s14, null);
        List<Object> v14 = (List<Object>)v.s14;
        RRAssert.areEqual(v14.size(), 2);
        verify_testpod1_array((testpod1[])v14.get(0), 3, gen.get_uint32());
        verify_testpod1_array((testpod1[])v14.get(1), 5, gen.get_uint32());

        RRAssert.areNotEqual(v.s15, null);
        List<Object> v15 = (List<Object>)v.s15;
        RRAssert.areEqual(v15.size(), 2);
        verify_testpod1_multidimarray(v15.get(0), 7, 2, gen.get_uint32());
        verify_testpod1_multidimarray(v15.get(1), 5, 1, gen.get_uint32());

        verify_transform(v.t1, gen.get_uint32());

        verify_transform_array(v.t2, 4, gen.get_uint32());
        verify_transform_multidimarray(v.t3, 2, 4, gen.get_uint32());

        verify_transform_array((transform[])(v.t4), 10, gen.get_uint32());
        verify_transform_multidimarray((NamedMultiDimArray)(v.t5), 6, 5, gen.get_uint32());

        RRAssert.areNotEqual(v.t6, null);
        RRAssert.areEqual(v.t6.size(), 1);
        transform[] t6_0 = v.t6.get(0);
        verify_transform(t6_0[0], gen.get_uint32());

        RRAssert.areNotEqual(v.t7, null);
        RRAssert.areEqual(v.t7.size(), 2);
        verify_transform_array(v.t7.get(0), 4, gen.get_uint32());
        verify_transform_array(v.t7.get(1), 4, gen.get_uint32());

        RRAssert.areNotEqual(v.t8, null);
        RRAssert.areEqual(v.t8.size(), 2);
        verify_transform_multidimarray(v.t8.get(0), 2, 4, gen.get_uint32());
        verify_transform_multidimarray(v.t8.get(1), 2, 4, gen.get_uint32());

        RRAssert.areNotEqual(v.t9, null);
        Object t9_1 = ((List<Object>)v.t9).get(0);
        transform[] t9 = (transform[])t9_1;
        verify_transform(t9[0], gen.get_uint32());

        RRAssert.areNotEqual(v.t10, null);
        List<Object> t10 = (List<Object>)v.t10;
        RRAssert.areEqual(t10.size(), 2);
        verify_transform_array((transform[])t10.get(0), 3, gen.get_uint32());
        verify_transform_array((transform[])t10.get(1), 5, gen.get_uint32());

        RRAssert.areNotEqual(v.t11, null);
        List<Object> t11 = (List<Object>)v.t11;
        RRAssert.areEqual(t11.size(), 2);
        verify_transform_multidimarray((NamedMultiDimArray)t11.get(0), 7, 2, gen.get_uint32());
        verify_transform_multidimarray((NamedMultiDimArray)t11.get(1), 5, 1, gen.get_uint32());
    }

    public static void fill_transform(transform t, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        double[] a = new double[7];
        for (int i = 0; i < 7; i++)
            a[i] = gen.get_double();
        t.assignFromNumericArray(a, 0);
    }

    public static void verify_transform(transform t, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        double[] a = t.getNumericArray();
        for (int i = 0; i < 7; i++)
            RRAssert.areEqual(a[i], gen.get_double());
    }

    public static transform[] fill_transform_array(int len, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        transform[] o = new transform[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = new transform();
            fill_transform(o[i], gen.get_uint32());
        }
        return o;
    }

    public static void verify_transform_array(transform[] t, int len, long seed)
    {
        ServiceTest2_test_sequence_gen gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.areEqual(t.length, len);
        for (int i = 0; i < len; i++)
            verify_transform(t[i], gen.get_uint32());
    }

    public static NamedMultiDimArray fill_transform_multidimarray(int m, int n, long seed)
    {
        return new NamedMultiDimArray(new int[] {m, n}, fill_transform_array(m * n, seed));
    }

    public static void verify_transform_multidimarray(NamedMultiDimArray a, int m, int n, long seed)
    {
        RRAssert.isTrue(Arrays.equals(a.dims, new int[] {m, n}));
        verify_transform_array((transform[])a.namedarray_array, m * n, seed);
    }
}
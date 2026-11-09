package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;

import java.util.*;

public class testroot3_impl implements testroot3
{

    Wire<int[]> _peekwire;
    Wire<int[]> _pokewire;
    WireBroadcaster<int[]> _peekwire_b;
    WireUnicastReceiver<int[]> _pokewire_r;

    com.robotraconteur.Timer _peekwire_t;

    public Wire<int[]> get_peekwire()
    {
        return _peekwire;
    }
    public void set_peekwire(Wire<int[]> value)
    {
        _peekwire = value;
        _peekwire_b = new WireBroadcaster<int[]>(_peekwire);
        int[] v = new int[] {56295674};
        _peekwire_b.setOutValue(v);
        _peekwire_t = RobotRaconteurNode.s().createTimer(100, new peektimer_handler());
    }
    public Wire<int[]> get_pokewire()
    {
        return _pokewire;
    }
    public void set_pokewire(Wire<int[]> value)
    {
        _pokewire = value;
        _pokewire_r = new WireUnicastReceiver<int[]>(_pokewire);
        _pokewire_r.addInValueListener(new pokewire_changed());
    }

    class peektimer_handler implements Action1<TimerEvent>
    {
        @Override public void action(TimerEvent evt)
        {
            int[] v = new int[] {56295674};
            _peekwire_b.setOutValue(v);
        }
    }

    class pokewire_changed implements Action3<int[], TimeSpec, Long>
    {
        public void action(int[] value, TimeSpec ts, Long ep)
        {
            System.out.println("RobotRaconteurTestService2.pokewire changed: " + Integer.toString(value[0]));
        }
    }

    public int get_unknown_modifier()
    {
        return 0;
    }

    public void set_unknown_modifier(int v)
    {}

    public ArrayMemory<double[]> get_readmem()
    {
        return null;
    }

    Pipe<int[]> unreliable1;

    public Pipe<int[]> get_unreliable1()
    {
        return unreliable1;
    }

    public void set_unreliable1(Pipe<int[]> p)
    {
        unreliable1 = p;
    }

    public Pipe<int[]> get_unreliable2()
    {
        return unreliable1;
    }

    public void set_unreliable2(Pipe<int[]> p)
    {
        unreliable1 = p;
    }

    public int get_readme()
    {
        return 0;
    }

    public void set_readme(int v)
    {}

    public int get_writeme()
    {
        return 0;
    }

    public void set_writeme(int v)
    {}

    public testenum1 get_testenum1_prop()
    {
        return testenum1.anothervalue;
    }
    public void set_testenum1_prop(testenum1 value)
    {
        RRAssert.areEqual(value, testenum1.hexval1);
    }
    public obj4 get_o4()
    {
        return new obj4_impl();
    }
    @Override public testpod1 get_testpod1_prop()
    {
        testpod1 o = new testpod1();
        ServiceTest2_pod.fill_testpod1(o, 563921043);
        return o;
    }
    @Override public void set_testpod1_prop(testpod1 value)
    {
        ServiceTest2_pod.verify_testpod1(value, 85932659);
    }
    @Override public teststruct3 get_teststruct3_prop()
    {
        return ServiceTest2_pod.fill_teststruct3(16483675);
    }
    @Override public void set_teststruct3_prop(teststruct3 value)
    {
        ServiceTest2_pod.verify_teststruct3(value, 858362);
    }
    @Override public void testpod1_func1(testpod1 s)
    {
        ServiceTest2_pod.verify_testpod1(s, 29546592);
    }
    @Override public testpod1 testpod1_func2()
    {
        testpod1 o = new testpod1();
        ServiceTest2_pod.fill_testpod1(o, 95836295);
        return o;
    }
    @Override public Generator2<double[]> gen_func1()
    {
        ArrayList<double[]> v = new ArrayList<double[]>();
        for (double i = 0; i < 16; i++)
        {
            v.add(new double[] {i});
        }
        return new IteratorGenerator<double[]>(v.iterator());
    }
    @Override public Generator2<UnsignedBytes> gen_func2(String name)
    {
        ArrayList<UnsignedBytes> v = new ArrayList<UnsignedBytes>();
        for (byte i = 0; i < 16; i++)
        {
            v.add(new UnsignedBytes(new byte[] {i}));
        }

        return new IteratorGenerator<UnsignedBytes>(v.iterator());
    }
    @Override public Generator3<UnsignedBytes> gen_func3(String name)
    {
        // TODO Auto-generated method stub
        return null;
    }

    class func4_gen extends SyncGenerator1<UnsignedBytes, UnsignedBytes>
    {

        boolean aborted = false;
        byte j = 0;

        @Override public void close()
        {
            j = 100;
        }

        @Override public void abort()
        {
            aborted = true;
        }

        @Override public UnsignedBytes next(UnsignedBytes param)
        {
            if (aborted)
            {
                throw new OperationAbortedException("");
            }

            if (j >= 8)
            {
                throw new StopIterationException("");
            }
            byte[] a = new byte[param.value.length];
            for (int i = 0; i < param.value.length; i++)
            {
                a[i] = (byte)(param.value[i] + j);
            }
            j++;
            return new UnsignedBytes(a);
        }
    }

    @Override public Generator1<UnsignedBytes, UnsignedBytes> gen_func4()
    {

        return new func4_gen();
    }
    @Override public Generator1<teststruct2, teststruct2> gen_func5()
    {
        // TODO Auto-generated method stub
        return null;
    }

    PodArrayMemory<testpod2[]> m_pod_m1 = new PodArrayMemory<testpod2[]>(new testpod2[1024]);

    @Override public PodArrayMemory<testpod2[]> get_pod_m1()
    {
        return m_pod_m1;
    }

    PodMultiDimArrayMemory<testpod2[]> m_pod_m2 =
        new PodMultiDimArrayMemory<testpod2[]>(new PodMultiDimArray(new int[] {3, 3}, new testpod2[9]));
    @Override public PodMultiDimArrayMemory<testpod2[]> get_pod_m2()
    {
        return m_pod_m2;
    }
    @Override public List<double[]> get_d1()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d1(List<double[]> value)
    {
        // TODO Auto-generated method stub
    }
    @Override public List<double[]> get_d2()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d2(List<double[]> value)
    {
        // TODO Auto-generated method stub
    }
    @Override public Map<Integer, double[]> get_d3()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d3(Map<Integer, double[]> value)
    {
        // TODO Auto-generated method stub
    }
    @Override public Map<Integer, double[]> get_d4()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d4(Map<Integer, double[]> value)
    {
        // TODO Auto-generated method stub
    }
    @Override public List<MultiDimArray> get_d5()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d5(List<MultiDimArray> value)
    {
        // TODO Auto-generated method stub
    }
    @Override public Map<Integer, MultiDimArray> get_d6()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public void set_d6(Map<Integer, MultiDimArray> value)
    {
        // TODO Auto-generated method stub
    }

    Pipe<int[]> p1;
    @Override public Pipe<int[]> get_p1()
    {
        return p1;
    }
    @Override public void set_p1(Pipe<int[]> value)
    {
        p1 = value;
    }
    Pipe<int[]> p2;
    @Override public Pipe<int[]> get_p2()
    {
        return p2;
    }
    @Override public void set_p2(Pipe<int[]> value)
    {
        p2 = value;
    }
    Pipe<MultiDimArray> p3;
    @Override public Pipe<MultiDimArray> get_p3()
    {
        return p3;
    }
    @Override public void set_p3(Pipe<MultiDimArray> value)
    {
        p3 = value;
    }
    Wire<int[]> w1;
    @Override public Wire<int[]> get_w1()
    {
        return w1;
    }
    @Override public void set_w1(Wire<int[]> value)
    {
        w1 = value;
    }
    Wire<int[]> w2;
    @Override public Wire<int[]> get_w2()
    {
        return w2;
    }
    @Override public void set_w2(Wire<int[]> value)
    {
        w2 = value;
    }

    Wire<MultiDimArray> w3;
    @Override public Wire<MultiDimArray> get_w3()
    {
        return w3;
    }
    @Override public void set_w3(Wire<MultiDimArray> value)
    {
        w3 = value;
    }
    @Override public vector3 get_testnamedarray1()
    {
        transform a1 = new transform();
        ServiceTest2_pod.fill_transform(a1, 74637);
        return a1.translation;
    }
    @Override public void set_testnamedarray1(vector3 value)
    {
        transform a1 = new transform();
        ServiceTest2_pod.fill_transform(a1, 3956378);
        a1.translation = value;
        ServiceTest2_pod.verify_transform(a1, 3956378);
    }
    @Override public transform get_testnamedarray2()
    {
        transform a2 = new transform();
        ServiceTest2_pod.fill_transform(a2, 1294);
        return a2;
    }
    @Override public void set_testnamedarray2(transform value)
    {
        ServiceTest2_pod.verify_transform(value, 827635);
    }
    @Override public transform[] get_testnamedarray3()
    {
        return ServiceTest2_pod.fill_transform_array(8, 837512);
    }
    @Override public void set_testnamedarray3(transform[] value)
    {
        ServiceTest2_pod.verify_transform_array(value, 6, 19274);
    }
    @Override public NamedMultiDimArray get_testnamedarray4()
    {
        return ServiceTest2_pod.fill_transform_multidimarray(7, 2, 66134);
    }
    @Override public void set_testnamedarray4(NamedMultiDimArray value)
    {
        ServiceTest2_pod.verify_transform_multidimarray(value, 5, 2, 6385);
    }
    @Override public NamedMultiDimArray get_testnamedarray5()
    {
        return ServiceTest2_pod.fill_transform_multidimarray(3, 2, 773142);
    }
    @Override public void set_testnamedarray5(NamedMultiDimArray value)
    {
        ServiceTest2_pod.verify_transform_multidimarray(value, 3, 2, 7732);
    }

    NamedArrayMemory<transform[]> m_namedarray_m1 = new NamedArrayMemory<transform[]>(new transform[512]);

    @Override public NamedArrayMemory<transform[]> get_namedarray_m1()
    {
        return m_namedarray_m1;
    }

    NamedMultiDimArrayMemory<transform[]> m_namedarray_m2 =
        new NamedMultiDimArrayMemory<transform[]>(new NamedMultiDimArray(new int[] {3, 3}, new transform[9]));

    @Override public NamedMultiDimArrayMemory<transform[]> get_namedarray_m2()
    {
        return m_namedarray_m2;
    }

    public final void ca(CDouble[] v1, CDouble[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(CDouble[] v1, CDouble[] v2, int len)
    {
        RRAssert.areEqual(v1.length, v2.length);

        for (int i = 0; i < len; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public final void ca(CSingle[] v1, CSingle[] v2)
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

    public final void ca(boolean[] v1, boolean[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    static CDouble[] complexFromScalars(double[] a)
    {
        CDouble[] o = new CDouble[a.length / 2];
        for (int j = 0; j < o.length; j++)
            o[j] = new CDouble(a[j * 2], a[j * 2 + 1]);
        return o;
    }

    static CSingle[] complexFromScalars(float[] a)
    {
        CSingle[] o = new CSingle[a.length / 2];
        for (int j = 0; j < o.length; j++)
            o[j] = new CSingle(a[j * 2], a[j * 2 + 1]);
        return o;
    }

    @Override public CDouble get_c1()
    {
        return new CDouble(5.708705e+01, -2.328294e-03);
    }
    @Override public void set_c1(CDouble value)
    {
        CDouble c1_2 = new CDouble(5.708705e+01, -2.328294e-03);
        RRAssert.areEqual(value, c1_2);
    }
    @Override public CDouble[] get_c2()
    {
        double[] c2_1_1 = TestVectors.getDoubles("ts3.c2.to_client");
        return complexFromScalars(c2_1_1);
    }
    @Override public void set_c2(CDouble[] value)
    {
        double[] c2_2_1 = TestVectors.getDoubles("ts3.c2.to_service");

        ca(complexFromScalars(c2_2_1), value);
    }
    @Override public MultiDimArray get_c3()
    {
        int[] c3_1_1 = TestVectors.getUInt32s("ts3.c3.to_client.dims");
        double[] c3_1_2 = TestVectors.getDoubles("ts3.c3.to_client");
        return new MultiDimArray(c3_1_1, complexFromScalars(c3_1_2));
    }
    @Override public void set_c3(MultiDimArray value)
    {
        int[] c3_2_1 = TestVectors.getUInt32s("ts3.c3.to_service.dims");
        double[] c3_2_2 = TestVectors.getDoubles("ts3.c3.to_service");
        ca(value.dims, c3_2_1);
        ca((CDouble[])value.array, complexFromScalars(c3_2_2));
    }
    @Override public List<CDouble[]> get_c4()
    {
        throw new UnsupportedOperationException();
    }
    @Override public void set_c4(List<CDouble[]> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public List<CDouble[]> get_c5()
    {
        List<CDouble[]> c5_1 = new ArrayList<CDouble[]>();
        double[] c5_1_1 = TestVectors.getDoubles("ts3.c5.to_client");
        c5_1.add(complexFromScalars(c5_1_1));
        return c5_1;
    }
    @Override public void set_c5(List<CDouble[]> value)
    {
        double[] c5_2_1 = TestVectors.getDoubles("ts3.c5.to_service");
        ca(value.get(0), complexFromScalars(c5_2_1));
    }
    @Override public List<MultiDimArray> get_c6()
    {
        throw new UnsupportedOperationException();
    }
    @Override public void set_c6(List<MultiDimArray> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public CSingle get_c7()
    {
        return new CSingle(-5.527021e-18f, -9.848457e+03f);
    }
    @Override public void set_c7(CSingle value)
    {
        CSingle c7_2 = new CSingle(9.303345e-12f, -3.865684e-05f);
        RRAssert.areEqual(value, c7_2);
    }
    @Override public CSingle[] get_c8()
    {
        float[] c8_1_1 = TestVectors.getFloats("ts3.c8.to_client");
        return complexFromScalars(c8_1_1);
    }
    @Override public void set_c8(CSingle[] value)
    {
        float[] c8_2_1 = TestVectors.getFloats("ts3.c8.to_service");
        ca(value, complexFromScalars(c8_2_1));
    }
    @Override public MultiDimArray get_c9()
    {
        int[] c9_1_1 = TestVectors.getUInt32s("ts3.c9.to_client.dims");
        float[] c9_1_2 = TestVectors.getFloats("ts3.c9.to_client");
        return new MultiDimArray(c9_1_1, complexFromScalars(c9_1_2));
    }
    @Override public void set_c9(MultiDimArray value)
    {
        int[] c9_2_1 = TestVectors.getUInt32s("ts3.c9.to_service.dims");
        float[] c9_2_2 = TestVectors.getFloats("ts3.c9.to_service");
        ca(value.dims, c9_2_1);
        ca((CSingle[])value.array, complexFromScalars(c9_2_2));
    }
    @Override public List<CSingle[]> get_c10()
    {
        throw new UnsupportedOperationException();
    }
    @Override public void set_c10(List<CSingle[]> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public List<CSingle[]> get_c11()
    {
        throw new UnsupportedOperationException();
    }
    @Override public void set_c11(List<CSingle[]> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public List<MultiDimArray> get_c12()
    {
        throw new UnsupportedOperationException();
    }
    @Override public void set_c12(List<MultiDimArray> value)
    {
        throw new UnsupportedOperationException();
    }
    @Override public boolean get_b1()
    {
        return true;
    }
    @Override public void set_b1(boolean value)
    {
        RRAssert.areEqual(value, true);
    }
    @Override public boolean[] get_b2()
    {
        return new boolean[] {true, false, true, true, false, true, false};
    }
    @Override public void set_b2(boolean[] value)
    {
        ca(value, new boolean[] {true, false, false, true, true, true, false, true});
    }
    @Override public MultiDimArray get_b3()
    {
        return new MultiDimArray(new int[] {2, 2}, new boolean[] {false, true, true, false});
    }
    @Override public void set_b3(MultiDimArray value)
    {
        ca(value.dims, new int[] {2, 1});
        ca((boolean[])value.array, new boolean[] {true, false});
    }
    @Override public List<boolean[]> get_b4()
    {
        List<boolean[]> o = new ArrayList<boolean[]>();
        o.add(new boolean[] {true});
        return o;
    }
    @Override public void set_b4(List<boolean[]> value)
    {
        RRAssert.areEqual(value.get(0)[0], true);
    }
    @Override public List<boolean[]> get_b5()
    {
        List<boolean[]> o = new ArrayList<boolean[]>();
        o.add(new boolean[] {false, true, false, false});
        return o;
    }
    @Override public void set_b5(List<boolean[]> value)
    {
        ca(value.get(0), new boolean[] {true, false});
    }
    @Override public List<MultiDimArray> get_b6()
    {
        List<MultiDimArray> o = new ArrayList<MultiDimArray>();
        o.add(new MultiDimArray(new int[] {2, 2}, new boolean[] {false, true, true, false}));
        return o;
    }
    @Override public void set_b6(List<MultiDimArray> value)
    {
        ca(value.get(0).dims, new int[] {2, 1});
        ca((boolean[])value.get(0).array, new boolean[] {true, false});
    }

    ArrayMemory<CDouble[]> c_m1 = new ArrayMemory<CDouble[]>(new CDouble[512]);
    MultiDimArrayMemory<CDouble[]> c_m2 =
        new MultiDimArrayMemory<CDouble[]>(new MultiDimArray(new int[] {10, 10}, new CDouble[100]));

    @Override public ArrayMemory<CDouble[]> get_c_m1()
    {
        return c_m1;
    }
    @Override public MultiDimArrayMemory<CDouble[]> get_c_m2()
    {
        return c_m2;
    }
    @Override public ArrayMemory<CDouble[]> get_c_m3()
    {
        // TODO Auto-generated method stub
        return null;
    }
    @Override public MultiDimArrayMemory<CDouble[]> get_c_m4()
    {
        // TODO Auto-generated method stub
        return null;
    }

    ArrayMemory<boolean[]> c_m5 = new ArrayMemory<boolean[]>(new boolean[512]);
    MultiDimArrayMemory<boolean[]> c_m6 =
        new MultiDimArrayMemory<boolean[]>(new MultiDimArray(new int[] {10, 10}, new boolean[100]));

    @Override public ArrayMemory<boolean[]> get_c_m5()
    {
        return c_m5;
    }
    @Override public MultiDimArrayMemory<boolean[]> get_c_m6()
    {
        return c_m6;
    }

    @Override public obj5 get_nolock_test()
    {
        return o5;
    }

    obj5_impl o5 = new obj5_impl();

    @Override public void test_exception_params1()
    {
        HashMap<String, Object> params_ = new HashMap<String, Object>();
        params_.put("param1", new int[] {10});
        params_.put("param2", "20");
        throw new RobotRaconteurException(MessageErrorType.MessageErrorType_InvalidOperation,
                                          "RobotRaconteur.InvalidOperation", "test error", "my_error", params_);
    }

    @Override public void test_exception_params2()
    {
        HashMap<String, Object> params_ = new HashMap<String, Object>();
        params_.put("param1", new int[] {30});
        params_.put("param2", "40");
        throw new com.robotraconteur.testing.TestService3.test_exception4("test error2", "my_error2", params_);
    }

    public Generator2<testenum1[]> enum_generator1()
    {
        throw new UnsupportedOperationException();
    }
    public Generator1<testenum1[], testenum1[]> enum_generator2(int a, int b)
    {
        throw new UnsupportedOperationException();
    }
}

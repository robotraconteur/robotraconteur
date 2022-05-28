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
        double[] c2_1_1 = new double[] {
            1.968551e+07,  2.380643e+18,  3.107374e-16,  7.249542e-16,  -4.701135e-19, -6.092764e-17, 2.285854e+14,
            2.776180e+05,  -1.436152e-12, 3.626609e+11,  3.600952e-02,  -3.118123e-16, -1.312210e-10, -1.738940e-07,
            -1.476586e-12, -2.899781e-20, 4.806642e+03,  4.476869e-05,  -2.935084e-16, 3.114019e-20,  -3.675955e+01,
            3.779796e-21,  2.190594e-11,  4.251420e-06,  -9.715221e+11, -3.483924e-01, 7.606428e+05,  5.418088e+15,
            4.786378e+16,  -1.202581e+08, -1.662061e+02, -2.392954e+03};
        return complexFromScalars(c2_1_1);
    }
    @Override public void set_c2(CDouble[] value)
    {
        double[] c2_2_1 = new double[] {
            4.925965e-03,  5.695254e+13,  -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02,
            -1.954145e+04, -2.499019e-16, 4.010614e+09,  -1.906811e-08, 3.297924e-10,  2.742399e-02,  -4.372839e-01,
            -3.093171e-10, 4.311755e-01,  -2.218220e-14, 5.399758e+10,  3.360304e+17,  1.340681e-18,  -4.441140e+11,
            -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14,  1.179639e+15,
            6.976204e-10,  1.901856e+08,  -3.824351e-02, -1.414167e+08};

        ca(complexFromScalars(c2_2_1), value);
    }
    @Override public MultiDimArray get_c3()
    {
        int[] c3_1_1 = new int[] {2, 5};
        double[] c3_1_2 = new double[] {5.524802e+18,  -2.443857e-05, 3.737932e-02,  -4.883553e-03, -1.184347e+12,
                                        4.537366e-08,  -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12,
                                        -2.537376e-17, 1.835687e-10,  -9.366069e-22, -5.426323e-12, -7.820969e-10,
                                        -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07,  1.860782e-16};
        return new MultiDimArray(c3_1_1, complexFromScalars(c3_1_2));
    }
    @Override public void set_c3(MultiDimArray value)
    {
        int[] c3_2_1 = new int[] {3, 4};
        double[] c3_2_2 =
            new double[] {4.435180e+04,  5.198060e-18,  -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09,
                          -6.332363e-11, -1.128999e+16, 4.869912e+16,  2.680490e-04,  -8.880119e-04, 3.960452e+11,
                          4.427784e-09,  -2.813742e-18, 7.397516e+18,  1.196394e+13,  3.236906e-14,  -4.219297e-17,
                          1.316282e-06,  -2.771084e-18, -1.239118e-09, 2.887453e-08,  -1.746515e+08, -2.312264e-11};
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
        double[] c5_1_1 =
            new double[] {1.104801e+00,  4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01,
                          -2.026434e+06, 4.450447e-19, 3.702953e-21,  9.722025e+12, 3.464073e-14, 4.628110e+15,
                          2.345453e-19,  3.730012e-04, 4.116650e+16,  4.380220e+08};
        c5_1.add(complexFromScalars(c5_1_1));
        return c5_1;
    }
    @Override public void set_c5(List<CDouble[]> value)
    {
        double[] c5_2_1 = {2.720831e-20,  2.853037e-16,  -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12,
                           -3.657056e+11, 2.718388e+15,  1.597672e+03,  2.611859e+14,  2.224926e+06,  -1.431096e-09,
                           3.699894e+19,  -5.936706e-01, -1.385395e-09, -4.248415e-13};
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
        float[] c8_1_1 =
            new float[] {-3.153395e-09f, 3.829492e-02f,  -2.665239e+12f, 1.592927e-03f,  3.188444e+06f,  -3.595015e-11f,
                         2.973887e-18f,  -2.189921e+17f, 1.651567e+10f,  1.095838e+05f,  3.865249e-02f,  4.725510e+10f,
                         -2.334376e+03f, 3.744977e-05f,  -1.050821e+02f, 1.122660e-22f,  3.501520e-18f,  -2.991601e-17f,
                         6.039622e-17f,  4.778095e-07f,  -4.793136e-05f, 3.096513e+19f,  2.476004e+18f,  1.296297e-03f,
                         2.165336e-13f,  4.834427e+06f,  4.675370e-01f,  -2.942290e-12f, -2.090883e-19f, 6.674942e+07f,
                         -4.809047e-10f, -4.911772e-13f};
        return complexFromScalars(c8_1_1);
    }
    @Override public void set_c8(CSingle[] value)
    {
        float[] c8_2_1 =
            new float[] {1.324498e+06f,  1.341746e-04f,  4.292993e-04f,  -3.844509e+15f, -3.804802e+10f, 3.785305e-12f,
                         2.628285e-19f,  -1.664089e+15f, -4.246472e-10f, -3.334943e+03f, -3.305796e-01f, 1.878648e-03f,
                         1.420880e-05f,  -3.024657e+14f, 2.227031e-21f,  2.044653e+17f,  9.753609e-20f,  -6.581817e-03f,
                         3.271063e-03f,  -1.726081e+06f, -1.614502e-06f, -2.641638e-19f, -2.977317e+07f, -1.278224e+03f,
                         -1.760207e-05f, -4.877944e-07f, -2.171524e+02f, 1.620645e+01f,  -4.334168e-02f, 1.871011e-09f,
                         -3.066163e+06f, -3.533662e+07f};
        ca(value, complexFromScalars(c8_2_1));
    }
    @Override public MultiDimArray get_c9()
    {
        int[] c9_1_1 = new int[] {2, 4};
        float[] c9_1_2 =
            new float[] {1.397743e+15f,  3.933042e+10f, -3.812329e+07f, 1.508109e+16f,  -2.091397e-20f, 3.207851e+12f,
                         -3.640702e+02f, 3.903769e+02f, -2.879727e+17f, -4.589604e-06f, 2.202769e-06f,  2.892523e+04f,
                         -3.306489e-14f, 4.522308e-06f, 1.665807e+15f,  2.340476e+10f};
        return new MultiDimArray(c9_1_1, complexFromScalars(c9_1_2));
    }
    @Override public void set_c9(MultiDimArray value)
    {
        int[] c9_2_1 = new int[] {2, 2, 2};
        float[] c9_2_2 =
            new float[] {2.138322e-03f,  4.036979e-21f,  1.345236e+10f, -1.348460e-12f, -3.615340e+12f, -2.911340e-21f,
                         3.220362e+09f,  3.459909e-04f,  4.276259e-08f, -3.199451e+18f, 3.468308e+07f,  -2.928506e-09f,
                         -3.154288e+17f, -2.352920e-02f, 6.976385e-21f, 2.435472e+12f};
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
package com.robotraconteur.javatest;

import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;

import java.util.*;

import com.robotraconteur.*;
import com.robotraconteur.Pipe.PipeEndpoint;

import java.util.concurrent.*;

public class ServiceTestClient2
{

    public final void RunFullTest(String url)
    {
        connectService(url);

        RRAssert.areEqual(r.get_testenum1_prop(), testenum1.anothervalue);

        r.set_testenum1_prop(testenum1.hexval1);
        r.get_o4();
        testWirePeekPoke();
        asyncTestWirePeekPoke();

        testEnums();
        testPods();

        testGenerators();
        testMemories();

        testNamedArrays();

        testNamedArrayMemories();

        testComplex();

        testComplexMemories();

        testNoLock();

        testBools();
        testBoolMemories();

        testExceptionParams();

        disconnectService();
    }

    private testroot3 r;

    public final void connectService(String url)
    {
        r = (testroot3)RobotRaconteurNode.s().connectService(url);
    }

    public final void disconnectService()
    {
        RobotRaconteurNode.s().disconnectService(r);
    }

    public void testWirePeekPoke()
    {
        int[] v = r.get_peekwire().peekInValue();
        RRAssert.areEqual(v[0], 56295674);

        int[] v_poke1 = new int[] {75738265};

        r.get_pokewire().pokeOutValue(v_poke1);
        int[] v2 = r.get_pokewire().peekOutValue();
        RRAssert.areEqual(v2[0], 75738265);

        Wire<int[]>.WireConnection w = r.get_pokewire().connect();
        for (int i = 0; i < 3; i++)
        {
            int[] v_setvalue1 = new int[] {8638356};
            w.setOutValue(v_setvalue1);
        }

        try
        {
            Thread.sleep(100);
        }
        catch (InterruptedException exp)
        {}

        int[] v3 = r.get_pokewire().peekOutValue();
        RRAssert.areEqual(v3[0], 8638356);
    }

    Object async_err_lock = new Object();
    RuntimeException async_err = null;
    CountDownLatch async_wait = new CountDownLatch(1);

    protected final void testAsync_err(Exception exp)
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

    public void asyncTestWirePeekPoke()
    {
        r.get_peekwire().asyncPeekInValue(new TestAsyncPeekPoke1());

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

    protected class TestAsyncPeekPoke1 implements Action3<int[], TimeSpec, RuntimeException>
    {
        public void action(int[] value, TimeSpec ts, RuntimeException exp)
        {
            if (exp != null)
            {
                testAsync_err(exp);
                return;
            }

            try
            {
                RRAssert.areEqual(value[0], 56295674);

                int[] v_out = new int[] {75738261};
                r.get_pokewire().asyncPokeOutValue(v_out, new TestAsyncPeekPoke2());
            }
            catch (Exception e)
            {
                testAsync_err(e);
            }
        }
    }

    protected class TestAsyncPeekPoke2 implements Action1<RuntimeException>
    {
        public void action(RuntimeException exp)
        {
            if (exp != null)
            {
                testAsync_err(exp);
                return;
            }

            try
            {
                r.get_pokewire().asyncPeekOutValue(new TestAsyncPeekPoke3());
            }
            catch (Exception e)
            {
                testAsync_err(e);
            }
        }
    }

    protected class TestAsyncPeekPoke3 implements Action3<int[], TimeSpec, RuntimeException>
    {
        public void action(int[] value, TimeSpec ts, RuntimeException exp)
        {
            if (exp != null)
            {
                testAsync_err(exp);
                return;
            }

            try
            {
                RRAssert.areEqual(value[0], 75738261);

                async_wait.countDown();

                System.out.println("Async wire peek-poke test complete");
            }
            catch (Exception e)
            {
                testAsync_err(e);
            }
        }
    }

    public void testEnums()
    {
        RRAssert.areEqual(r.get_testenum1_prop(), testenum1.anothervalue);

        r.set_testenum1_prop(testenum1.hexval1);
    }

    public void testPods()
    {
        testpod1 s1 = new testpod1();
        ServiceTest2_pod.fill_testpod1(s1, 563921043);
        ServiceTest2_pod.verify_testpod1(s1, 563921043);

        MessageElementData s1_m = RobotRaconteurNode.s().packStructure(s1);
        testpod1 s1_1 = RobotRaconteurNode.s().<testpod1[]>unpackStructure(s1_m)[0];
        ServiceTest2_pod.verify_testpod1(s1_1, 563921043);

        teststruct3 s2 = ServiceTest2_pod.fill_teststruct3(858362);
        ServiceTest2_pod.verify_teststruct3(s2, 858362);
        MessageElementData s2_m = RobotRaconteurNode.s().packStructure(s2);
        teststruct3 s2_1 = RobotRaconteurNode.s().<teststruct3>unpackStructure(s2_m);
        ServiceTest2_pod.verify_teststruct3(s2_1, 858362);

        testpod1 p1 = r.get_testpod1_prop();
        ServiceTest2_pod.verify_testpod1(p1, 563921043);
        testpod1 p2 = new testpod1();
        ServiceTest2_pod.fill_testpod1(p2, 85932659);
        r.set_testpod1_prop(p2);

        testpod1 f1 = r.testpod1_func2();
        ServiceTest2_pod.verify_testpod1(f1, 95836295);
        testpod1 f2 = new testpod1();
        ServiceTest2_pod.fill_testpod1(f2, 29546592);
        r.testpod1_func1(f2);

        ServiceTest2_pod.verify_teststruct3(r.get_teststruct3_prop(), 16483675);
        r.set_teststruct3_prop(ServiceTest2_pod.fill_teststruct3(858362));
    }

    public void testGenerators()
    {
        Generator1<UnsignedBytes, UnsignedBytes> g = r.gen_func4();
        for (int i = 0; i < 3; i++)
        {
            g.next(new UnsignedBytes(new byte[] {}));
        }
        UnsignedBytes b2 = g.next(new UnsignedBytes(new byte[] {2, 3, 4}));
        g.abort();
        try
        {
            g.next(new UnsignedBytes(new byte[] {2, 3, 4}));
        }
        catch (OperationAbortedException e)
        {}

        Generator2<double[]> g2 = r.gen_func1();
        List<double[]> res = g2.nextAll();
        System.out.println(res);

        Generator2<double[]> g3 = r.gen_func1();
        g3.next();
        g3.abort();
        try
        {
            g3.next();
        }
        catch (OperationAbortedException e)
        {
            System.out.println("Caught operation aborted");
        }

        Generator2<double[]> g4 = r.gen_func1();
        g4.next();
        g4.close();
        try
        {
            g4.next();
        }
        catch (StopIterationException e)
        {
            System.out.println("Caught operation aborted");
        }
    }

    public void testMemories()
    {
        test_m1();
        test_m2();
    }

    public void test_m1()
    {
        testpod2[] o1 = new testpod2[32];

        for (int i = 0; i < o1.length; i++)
        {
            o1[i] = new testpod2();
            ServiceTest2_pod.fill_testpod2(o1[i], 59174 + i);
        }

        RRAssert.areEqual(r.get_pod_m1().length(), 1024);

        r.get_pod_m1().write(52, o1, 3, 17);

        testpod2[] o2 = new testpod2[32];

        r.get_pod_m1().read(53, o2, 2, 16);

        for (int i = 2; i < 16; i++)
        {
            ServiceTest2_pod.verify_testpod2(o2[i], 59174 + i + 2);
        }
    }

    public void test_m2()
    {
        PodMultiDimArray s = new PodMultiDimArray(new int[] {3, 3}, new testpod2[9]);

        for (int i = 0; i < 9; i++)
        {
            ((testpod2[])s.pod_array)[i] = new testpod2();
            ServiceTest2_pod.fill_testpod2(((testpod2[])s.pod_array)[i], 75721 + i);
        }

        r.get_pod_m2().write(new long[] {0, 0}, s, new long[] {0, 0}, new long[] {3, 3});

        PodMultiDimArray s2 = new PodMultiDimArray(new int[] {3, 3}, new testpod2[9]);

        r.get_pod_m2().read(new long[] {0, 0}, s2, new long[] {0, 0}, new long[] {3, 3});

        for (int i = 0; i < 9; i++)
        {
            ServiceTest2_pod.verify_testpod2(((testpod2[])s2.pod_array)[i], 75721 + i);
        }
    }

    public void testNamedArrays()
    {
        transform a1 = new transform();
        ServiceTest2_pod.fill_transform(a1, 3956378);
        r.set_testnamedarray1(a1.translation);

        transform a1_1 = new transform();
        a1_1.rotation = a1.rotation;
        a1_1.translation = r.get_testnamedarray1();
        transform a1_2 = new transform();
        ServiceTest2_pod.fill_transform(a1_2, 74637);
        a1_1.rotation = a1_2.rotation;
        ServiceTest2_pod.verify_transform(a1_1, 74637);

        transform a2 = new transform();
        ServiceTest2_pod.fill_transform(a2, 827635);
        r.set_testnamedarray2(a2);

        transform a2_1 = r.get_testnamedarray2();
        ServiceTest2_pod.verify_transform(a2_1, 1294);

        r.set_testnamedarray3(ServiceTest2_pod.fill_transform_array(6, 19274));
        ServiceTest2_pod.verify_transform_array(r.get_testnamedarray3(), 8, 837512);

        r.set_testnamedarray4(ServiceTest2_pod.fill_transform_multidimarray(5, 2, 6385));
        ServiceTest2_pod.verify_transform_multidimarray(r.get_testnamedarray4(), 7, 2, 66134);

        r.set_testnamedarray5(ServiceTest2_pod.fill_transform_multidimarray(3, 2, 7732));
        ServiceTest2_pod.verify_transform_multidimarray(r.get_testnamedarray5(), 3, 2, 773142);
    }

    public void testNamedArrayMemories()
    {
        test_namedarray_m1();
        test_namedarray_m2();
    }

    public void test_namedarray_m1()
    {
        transform[] s = new transform[32];
        for (long i = 0; i < s.length; i++)
        {
            s[(int)i] = new transform();
            ServiceTest2_pod.fill_transform(s[(int)i], 79174 + i);
        }

        RRAssert.areEqual(r.get_namedarray_m1().length(), 512);
        r.get_namedarray_m1().write(23, s, 3, 21);

        transform[] s2 = new transform[32];
        r.get_namedarray_m1().read(24, s2, 2, 18);

        for (long i = 2; i < 18; i++)
        {
            ServiceTest2_pod.verify_transform(s2[(int)i], 79174 + i + 2);
        }
    }

    public void test_namedarray_m2()
    {
        NamedMultiDimArray s = new NamedMultiDimArray(new int[] {3, 3}, new transform[9]);
        transform[] s_array = (transform[])s.namedarray_array;
        for (long i = 0; i < s_array.length; i++)
        {
            s_array[(int)i] = new transform();
            ServiceTest2_pod.fill_transform(s_array[(int)i], 15721 + i);
        }

        r.get_namedarray_m2().write(new long[] {0, 0}, s, new long[] {0, 0}, new long[] {3, 3});

        NamedMultiDimArray s2 = new NamedMultiDimArray(new int[] {3, 3}, new transform[9]);
        r.get_namedarray_m2().read(new long[] {0, 0}, s2, new long[] {0, 0}, new long[] {3, 3});

        transform[] s2_array = (transform[])s2.namedarray_array;
        for (long i = 0; i < s2_array.length; i++)
            ServiceTest2_pod.fill_transform(s2_array[(int)i], 15721 + i);
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

    void testComplex()
    {
        CDouble c1_1 = new CDouble(5.708705e+01, -2.328294e-03);
        RRAssert.areEqual(r.get_c1(), c1_1);

        CDouble c1_2 = new CDouble(5.708705e+01, -2.328294e-03);
        r.set_c1(c1_2);

        CDouble[] c2_1 = r.get_c2();
        double[] c2_1_1 = new double[] {
            1.968551e+07,  2.380643e+18,  3.107374e-16,  7.249542e-16,  -4.701135e-19, -6.092764e-17, 2.285854e+14,
            2.776180e+05,  -1.436152e-12, 3.626609e+11,  3.600952e-02,  -3.118123e-16, -1.312210e-10, -1.738940e-07,
            -1.476586e-12, -2.899781e-20, 4.806642e+03,  4.476869e-05,  -2.935084e-16, 3.114019e-20,  -3.675955e+01,
            3.779796e-21,  2.190594e-11,  4.251420e-06,  -9.715221e+11, -3.483924e-01, 7.606428e+05,  5.418088e+15,
            4.786378e+16,  -1.202581e+08, -1.662061e+02, -2.392954e+03};
        ca(c2_1, complexFromScalars(c2_1_1));

        double[] c2_2_1 = new double[] {
            4.925965e-03,  5.695254e+13,  -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02,
            -1.954145e+04, -2.499019e-16, 4.010614e+09,  -1.906811e-08, 3.297924e-10,  2.742399e-02,  -4.372839e-01,
            -3.093171e-10, 4.311755e-01,  -2.218220e-14, 5.399758e+10,  3.360304e+17,  1.340681e-18,  -4.441140e+11,
            -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14,  1.179639e+15,
            6.976204e-10,  1.901856e+08,  -3.824351e-02, -1.414167e+08};

        r.set_c2(complexFromScalars(c2_2_1));

        MultiDimArray c3_1 = r.get_c3();
        int[] c3_1_1 = new int[] {2, 5};
        double[] c3_1_2 = new double[] {5.524802e+18,  -2.443857e-05, 3.737932e-02,  -4.883553e-03, -1.184347e+12,
                                        4.537366e-08,  -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12,
                                        -2.537376e-17, 1.835687e-10,  -9.366069e-22, -5.426323e-12, -7.820969e-10,
                                        -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07,  1.860782e-16};
        ca(c3_1.dims, c3_1_1);
        ca((CDouble[])c3_1.array, complexFromScalars(c3_1_2));

        int[] c3_2_1 = new int[] {3, 4};
        double[] c3_2_2 =
            new double[] {4.435180e+04,  5.198060e-18,  -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09,
                          -6.332363e-11, -1.128999e+16, 4.869912e+16,  2.680490e-04,  -8.880119e-04, 3.960452e+11,
                          4.427784e-09,  -2.813742e-18, 7.397516e+18,  1.196394e+13,  3.236906e-14,  -4.219297e-17,
                          1.316282e-06,  -2.771084e-18, -1.239118e-09, 2.887453e-08,  -1.746515e+08, -2.312264e-11};
        r.set_c3(new MultiDimArray(c3_2_1, complexFromScalars(c3_2_2)));

        List<CDouble[]> c5_1 = r.get_c5();
        double[] c5_1_1 =
            new double[] {1.104801e+00,  4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01,
                          -2.026434e+06, 4.450447e-19, 3.702953e-21,  9.722025e+12, 3.464073e-14, 4.628110e+15,
                          2.345453e-19,  3.730012e-04, 4.116650e+16,  4.380220e+08};
        ca(c5_1.get(0), complexFromScalars(c5_1_1));

        List<CDouble[]> c5_2 = new ArrayList<CDouble[]>();
        double[] c5_2_1 = {2.720831e-20,  2.853037e-16,  -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12,
                           -3.657056e+11, 2.718388e+15,  1.597672e+03,  2.611859e+14,  2.224926e+06,  -1.431096e-09,
                           3.699894e+19,  -5.936706e-01, -1.385395e-09, -4.248415e-13};
        c5_2.add(complexFromScalars(c5_2_1));
        r.set_c5(c5_2);

        CSingle c7_1 = new CSingle(-5.527021e-18f, -9.848457e+03f);
        RRAssert.areEqual(r.get_c7(), c7_1);

        CSingle c7_2 = new CSingle(9.303345e-12f, -3.865684e-05f);
        r.set_c7(c7_2);

        CSingle[] c8_1 = r.get_c8();
        float[] c8_1_1 =
            new float[] {-3.153395e-09f, 3.829492e-02f,  -2.665239e+12f, 1.592927e-03f,  3.188444e+06f,  -3.595015e-11f,
                         2.973887e-18f,  -2.189921e+17f, 1.651567e+10f,  1.095838e+05f,  3.865249e-02f,  4.725510e+10f,
                         -2.334376e+03f, 3.744977e-05f,  -1.050821e+02f, 1.122660e-22f,  3.501520e-18f,  -2.991601e-17f,
                         6.039622e-17f,  4.778095e-07f,  -4.793136e-05f, 3.096513e+19f,  2.476004e+18f,  1.296297e-03f,
                         2.165336e-13f,  4.834427e+06f,  4.675370e-01f,  -2.942290e-12f, -2.090883e-19f, 6.674942e+07f,
                         -4.809047e-10f, -4.911772e-13f};
        ca(c8_1, complexFromScalars(c8_1_1));

        float[] c8_2_1 =
            new float[] {1.324498e+06f,  1.341746e-04f,  4.292993e-04f,  -3.844509e+15f, -3.804802e+10f, 3.785305e-12f,
                         2.628285e-19f,  -1.664089e+15f, -4.246472e-10f, -3.334943e+03f, -3.305796e-01f, 1.878648e-03f,
                         1.420880e-05f,  -3.024657e+14f, 2.227031e-21f,  2.044653e+17f,  9.753609e-20f,  -6.581817e-03f,
                         3.271063e-03f,  -1.726081e+06f, -1.614502e-06f, -2.641638e-19f, -2.977317e+07f, -1.278224e+03f,
                         -1.760207e-05f, -4.877944e-07f, -2.171524e+02f, 1.620645e+01f,  -4.334168e-02f, 1.871011e-09f,
                         -3.066163e+06f, -3.533662e+07f};
        r.set_c8(complexFromScalars(c8_2_1));

        MultiDimArray c9_1 = r.get_c9();
        int[] c9_1_1 = new int[] {2, 4};
        float[] c9_1_2 =
            new float[] {1.397743e+15f,  3.933042e+10f, -3.812329e+07f, 1.508109e+16f,  -2.091397e-20f, 3.207851e+12f,
                         -3.640702e+02f, 3.903769e+02f, -2.879727e+17f, -4.589604e-06f, 2.202769e-06f,  2.892523e+04f,
                         -3.306489e-14f, 4.522308e-06f, 1.665807e+15f,  2.340476e+10f};
        ca(c9_1.dims, c9_1_1);
        ca((CSingle[])c9_1.array, complexFromScalars(c9_1_2));

        int[] c9_2_1 = new int[] {2, 2, 2};
        float[] c9_2_2 =
            new float[] {2.138322e-03f,  4.036979e-21f,  1.345236e+10f, -1.348460e-12f, -3.615340e+12f, -2.911340e-21f,
                         3.220362e+09f,  3.459909e-04f,  4.276259e-08f, -3.199451e+18f, 3.468308e+07f,  -2.928506e-09f,
                         -3.154288e+17f, -2.352920e-02f, 6.976385e-21f, 2.435472e+12f};
        r.set_c9(new MultiDimArray(c9_2_1, complexFromScalars(c9_2_2)));
    }

    void testComplexMemories()
    {
        double[] c_m1_1 =
            new double[] {8.952764e-05,  4.348213e-04,  -1.051215e+08, 1.458626e-09, -2.575954e+10, 2.118740e+03,
                          -2.555026e-02, 2.192576e-18,  -2.035082e+18, 2.951834e-09, -1.760731e+15, 4.620903e-11,
                          -3.098798e+05, -8.883556e-07, 2.472289e+17,  7.059075e-12};
        r.get_c_m1().write(10, complexFromScalars(c_m1_1), 0, 8);

        CDouble[] c_m1_3 = new CDouble[8];
        r.get_c_m1().read(10, c_m1_3, 0, 8);

        ca(complexFromScalars(c_m1_1), c_m1_3, 8);

        long[] z = new long[] {0, 0};
        long[] c = new long[] {3, 3};

        int[] c_m2_1 = new int[] {3, 3};
        double[] c_m2_2 =
            new double[] {-4.850043e-03, 3.545429e-07,  2.169430e+12,  1.175943e-09,  2.622300e+08, -4.439823e-11,
                          -1.520489e+17, 8.250078e-14,  3.835439e-07,  -1.424709e-02, 3.703099e+08, -1.971111e-08,
                          -2.805354e+01, -2.093850e-17, -4.476148e+19, 9.914350e+11,  2.753067e+08, -1.745041e+14};
        MultiDimArray c_m2_3 = new MultiDimArray(c_m2_1, complexFromScalars(c_m2_2));
        r.get_c_m2().write(z, c_m2_3, z, c);

        MultiDimArray c_m2_4 = new MultiDimArray(c_m2_1, new CDouble[9]);
        r.get_c_m2().read(z, c_m2_4, z, c);

        ca(c_m2_3.dims, c_m2_4.dims);
        ca((CDouble[])c_m2_3.array, (CDouble[])c_m2_4.array);
    }

    public void testNoLock()
    {
        obj5 o5 = r.get_nolock_test();
        boolean errthrown = false;
        try
        {
            o5.get_p1();
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        o5.get_p2();
        o5.set_p2(0);
        o5.get_p3();

        errthrown = false;
        try
        {
            o5.set_p3(0);
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        errthrown = false;
        try
        {
            o5.f1();
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        o5.f2();

        errthrown = false;
        try
        {
            o5.get_q1().connect(-1).close();
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        o5.get_q2().connect(-1).close();

        errthrown = false;
        try
        {
            o5.get_w1().connect().close();
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        o5.get_w2().connect().close();

        errthrown = false;
        try
        {
            o5.get_m1().length();
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);

        int[] b1 = new int[100];

        o5.get_m2().length();
        o5.get_m2().read(0, b1, 0, 10);
        o5.get_m2().write(0, b1, 0, 10);

        o5.get_m3().length();
        o5.get_m3().read(0, b1, 0, 10);

        errthrown = false;
        try
        {
            o5.get_m3().write(0, b1, 0, 10);
        }
        catch (ObjectLockedException e)
        {
            System.out.println(e.toString());
            errthrown = true;
        }
        RRAssert.isTrue(errthrown);
    }

    public void testBools()
    {
        r.set_b1(true);
        RRAssert.areEqual(r.get_b1(), true);

        r.set_b2(new boolean[] {true, false, false, true, true, true, false, true});
        ca(r.get_b2(), new boolean[] {true, false, true, true, false, true, false});

        MultiDimArray b3_1 = r.get_b3();
        ca(b3_1.dims, new int[] {2, 2});
        ca((boolean[])b3_1.array, new boolean[] {false, true, true, false});
        r.set_b3(new MultiDimArray(new int[] {2, 1}, new boolean[] {true, false}));

        List<boolean[]> b4_1 = r.get_b4();
        RRAssert.areEqual(b4_1.get(0)[0], true);
        List<boolean[]> b4_2 = new ArrayList<boolean[]>();
        b4_2.add(new boolean[] {true});
        r.set_b4(b4_2);

        List<boolean[]> b5_1 = r.get_b5();
        ca(b5_1.get(0), new boolean[] {false, true, false, false});
        List<boolean[]> b5_2 = new ArrayList<boolean[]>();
        b5_2.add(new boolean[] {true, false});
        r.set_b5(b5_2);

        List<MultiDimArray> b6_1 = r.get_b6();
        ca(b6_1.get(0).dims, new int[] {2, 2});
        ca((boolean[])b6_1.get(0).array, new boolean[] {false, true, true, false});
        List<MultiDimArray> b6_2 = new ArrayList<MultiDimArray>();
        b6_2.add(new MultiDimArray(new int[] {2, 1}, new boolean[] {true, false}));
        r.set_b6(b6_2);
    }

    public void testBoolMemories()
    {
        ArrayMemory<boolean[]> c_m5 = r.get_c_m5();
        boolean[] v1_1 = {true, false, false, true, true, false, false, false, true, true};
        c_m5.write(100, v1_1, 1, 8);
        boolean[] v2 = new boolean[10];
        c_m5.read(99, v2, 0, 10);
        for (int i = 1; i < 9; i++)
        {
            RRAssert.areEqual(v2[i], v1_1[i]);
        }

        MultiDimArrayMemory<boolean[]> c_m6 = r.get_c_m6();
        MultiDimArray v3 = new MultiDimArray(new int[] {2, 5}, v1_1);
        c_m6.write(new long[] {0, 0}, v3, new long[] {0, 0}, new long[] {2, 5});

        MultiDimArray v4 = new MultiDimArray(new int[] {2, 5}, new boolean[10]);
        c_m6.read(new long[] {0, 0}, v4, new long[] {0, 0}, new long[] {2, 5});
        ca(v3.dims, v4.dims);
        ca((boolean[])v3.array, (boolean[])v4.array);
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

    private void testExceptionParams()
    {

        boolean exp2_caught = false;
        try
        {
            r.test_exception_params2();
        }
        catch (com.robotraconteur.testing.TestService3.test_exception4 exp)
        {
            exp2_caught = true;
            RRAssert.areEqual(exp.getMessage(), "test error2");
            RRAssert.areEqual(exp.errorSubName, "my_error2");
            RRAssert.areNotEqual(exp.errorParam, null);
            HashMap<String, Object> param_map = (HashMap<String, Object>)(exp.errorParam);
            RRAssert.areEqual(param_map.size(), 2);
            RRAssert.areEqual(((int[])param_map.get("param1"))[0], 30);
            RRAssert.areEqual(((String)param_map.get("param2")), "40");
        }

        RRAssert.isTrue(exp2_caught);

        System.out.println("testExceptionParams complete");
    }
}
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using com.robotraconteur.testing.TestService3;
using System.Threading;
using com.robotraconteur.testing.TestService1;

namespace RobotRaconteurNETTest
{

public class RobotRaconteurTestServiceSupport2
{

    public testroot3_impl testservice2;

    public void RegisterServices(TcpTransport t)
    {
        testservice2 = new testroot3_impl();
        var context = RobotRaconteurNode.s.RegisterService("RobotRaconteurTestService2",
                                                           "com.robotraconteur.testing.TestService3", testservice2);
        context.RequestObjectLock("RobotRaconteurTestService2.nolock_test", "server");
    }

    public void UnregisterServices()
    {}
}

public class testroot3_impl : testroot3_default_impl, IRRServiceObject
{

    RobotRaconteur.Timer _peekwire_b_timer;

    public override Wire<int> peekwire
    {
        get {
            return base.peekwire;
        }
        set {
            base.peekwire = value;
            _peekwire_b_timer =
                RobotRaconteurNode.s.CreateTimer(100, delegate(TimerEvent ev) { rrvar_peekwire.OutValue = 56295674; });
            _peekwire_b_timer.Start();
            rrvar_peekwire.OutValue = 56295674;
        }
    }

    public void RRServiceObjectInit(ServerContext context, string service_path)
    {
        rrvar_pokewire.InValueChanged += delegate(int val, TimeSpec ts, uint client)
        {
            Console.WriteLine("PokeWire value changed: {0}", val);
            TimeSpec ts2;
            uint client2;
            Console.WriteLine("PokeWire InValue: {0}", rrvar_pokewire.GetInValue(out ts2, out client2));
            int val3;
            TimeSpec ts3;
            uint client3;
            if (rrvar_pokewire.TryGetInValue(out val3, out ts3, out client3))
            {
                Console.WriteLine("PokeWire TryGetInValue: {0}", val3);
            }
        };
    }

    public override Pipe<int> unreliable1 { get; set; }

    public override ArrayMemory<double> readmem { get; }
    public override testenum1 testenum1_prop
    {
        get {
            return testenum1.anothervalue;
        }
        set {
            RRAssert.AreEqual((uint)value, (uint)testenum1.hexval1);
        }
    }

    public override testpod1 testpod1_prop
    {
        get {
            var o = new testpod1();
            ServiceTest2_pod.fill_testpod1(ref o, 563921043);
            return o;
        }
        set {
            ServiceTest2_pod.verify_testpod1(ref value, 85932659);
        }
    }

    public override teststruct3 teststruct3_prop
    {
        get {
            return ServiceTest2_pod.fill_teststruct3(16483675);
        }
        set {
            ServiceTest2_pod.verify_teststruct3(value, 858362);
        }
    }

    // public PodArrayMemory<testpod2> pod_m1 { get => throw new System.NotImplementedException(); set => throw new
    // System.NotImplementedException(); } public PodMultiDimArrayMemory<testpod2> pod_m2 { get => throw new
    // System.NotImplementedException(); set => throw new System.NotImplementedException(); }

    public override Generator2<double> gen_func1()
    {
        return new EnumeratorGenerator<double>(new double[] { 5, 6, 7, 8 });
    }

    public override Generator2<byte[]> gen_func2(string name)
    {
        var o = new List<byte[]>();
        for (byte i = 0; i < 16; i++)
        {
            o.Add(new byte[] { i });
        }

        return new EnumeratorGenerator<byte[]>(o);
    }

    public override Generator3<byte[]> gen_func3(string name)
    {
        throw new System.NotImplementedException();
    }

    public override Generator1<byte[], byte[]> gen_func4()
    {
        return new func4_gen();
    }

    public override Generator1<teststruct2, teststruct2> gen_func5()
    {
        throw new System.NotImplementedException();
    }

    public override obj4 get_o4()
    {
        return new obj4_impl();
    }

    public override void testpod1_func1(testpod1 s)
    {
        ServiceTest2_pod.verify_testpod1(ref s, 29546592);
    }

    public override testpod1 testpod1_func2()
    {
        var o = new testpod1();
        ServiceTest2_pod.fill_testpod1(ref o, 95836295);
        return o;
    }

    PodArrayMemory<testpod2> m_pod_m1 = new PodArrayMemory<testpod2>(new testpod2[1024]);
    public override PodArrayMemory<testpod2> pod_m1
    {
        get {
            return m_pod_m1;
        }
    }

    PodMultiDimArrayMemory<testpod2> m_pod_m2 =
        new PodMultiDimArrayMemory<testpod2>(new PodMultiDimArray(new uint[] { 3, 3 }, new testpod2[9]));
    public override PodMultiDimArrayMemory<testpod2> pod_m2
    {
        get {
            return m_pod_m2;
        }
    }

    public override Wire<int[]> w1 { get; set; }
    public override Wire<int[]> w2 { get; set; }
    public override Wire<MultiDimArray> w3 { get; set; }
    public override Pipe<int[]> p1 { get; set; }
    public override Pipe<int[]> p2 { get; set; }
    public override Pipe<MultiDimArray> p3 { get; set; }

    public override vector3 testnamedarray1
    {
        get {
            var a1 = new transform();
            ServiceTest2_pod.fill_transform(ref a1, 74637);
            return a1.translation;
        }
        set {
            var a1 = new transform();
            ServiceTest2_pod.fill_transform(ref a1, 3956378);
            a1.translation = value;
            ServiceTest2_pod.verify_transform(ref a1, 3956378);
        }
    }
    public override transform testnamedarray2
    {
        get {
            var a2 = new transform();
            ServiceTest2_pod.fill_transform(ref a2, 1294);
            return a2;
        }
        set {
            ServiceTest2_pod.verify_transform(ref value, 827635);
        }
    }
    public override transform[] testnamedarray3
    {
        get {
            return ServiceTest2_pod.fill_transform_array(8, 837512);
        }
        set {
            ServiceTest2_pod.verify_transform_array(value, 6, 19274);
        }
    }
    public override NamedMultiDimArray testnamedarray4
    {
        get {
            return ServiceTest2_pod.fill_transform_multidimarray(7, 2, 66134);
        }
        set {
            ServiceTest2_pod.verify_transform_multidimarray(value, 5, 2, 6385);
        }
    }
    public override NamedMultiDimArray testnamedarray5
    {
        get {
            return ServiceTest2_pod.fill_transform_multidimarray(3, 2, 773142);
        }
        set {
            ServiceTest2_pod.verify_transform_multidimarray(value, 3, 2, 7732);
        }
    }

    NamedArrayMemory<transform> m_namedarray_m1 = new NamedArrayMemory<transform>(new transform[512]);
    public override NamedArrayMemory<transform> namedarray_m1
    {
        get {
            return m_namedarray_m1;
        }
    }

    NamedMultiDimArrayMemory<transform> m_namedarray_m2 =
        new NamedMultiDimArrayMemory<transform>(new NamedMultiDimArray(new uint[] { 3, 3 }, new transform[9]));
    public override NamedMultiDimArrayMemory<transform> namedarray_m2
    {
        get {
            return m_namedarray_m2;
        }
    }

    public void ca<T>(T[] v1, T[] v2)
        where T : IComparable, IComparable<T>
    {
        RRAssert.AreEqual(v1.Length, v2.Length);
        for (int i = 0; i < v1.Length; i++)
        {
            RRAssert.AreEqual(v1[i], v2[i]);
        }
    }

    public void ca(CDouble[] v1, CDouble[] v2)
    {
        RRAssert.AreEqual(v1.Length, v2.Length);
        for (int i = 0; i < v1.Length; i++)
        {
            RRAssert.AreEqual((object)v1[i], (object)v2[i]);
        }
    }

    public void ca(CSingle[] v1, CSingle[] v2)
    {
        RRAssert.AreEqual(v1.Length, v2.Length);
        for (int i = 0; i < v1.Length; i++)
        {
            RRAssert.AreEqual((object)v1[i], (object)v2[i]);
        }
    }

    static CDouble[] ComplexFromScalars(double[] a)
    {
        var o = new CDouble[a.Length / 2];
        for (int j = 0; j < o.Length; j++)
            o[j] = new CDouble(a[j * 2], a[j * 2 + 1]);
        return o;
    }

    static CSingle[] ComplexFromScalars(float[] a)
    {
        var o = new CSingle[a.Length / 2];
        for (int j = 0; j < o.Length; j++)
            o[j] = new CSingle(a[j * 2], a[j * 2 + 1]);
        return o;
    }

    public override CDouble c1
    {
        get {
            return new CDouble(5.708705e+01, -2.328294e-03);
        }
        set {
            var c1_2 = new CDouble(5.708705e+01, -2.328294e-03);
            RRAssert.AreEqual((object)value, (object)c1_2);
        }
    }
    public override CDouble[] c2
    {
        get {
            double[] c2_1_1 = TestVectors.Get<double>("ts3.c2.to_client");
            return ComplexFromScalars(c2_1_1);
        }
        set {
            double[] c2_2_1 = TestVectors.Get<double>("ts3.c2.to_service");

            ca(ComplexFromScalars(c2_2_1), value);
        }
    }
    public override MultiDimArray c3
    {
        get {
            uint[] c3_1_1 = TestVectors.Get<uint>("ts3.c3.to_client.dims");
            double[] c3_1_2 = TestVectors.Get<double>("ts3.c3.to_client");
            return new MultiDimArray(c3_1_1, ComplexFromScalars(c3_1_2));
        }
        set {
            uint[] c3_2_1 = TestVectors.Get<uint>("ts3.c3.to_service.dims");
            double[] c3_2_2 = TestVectors.Get<double>("ts3.c3.to_service");
            ca(value.Dims, c3_2_1);
            ca((CDouble[])value.Array_, ComplexFromScalars(c3_2_2));
        }
    }
    public override List<CDouble[]> c5
    {
        get {
            var c5_1 = new List<CDouble[]>();
            double[] c5_1_1 = TestVectors.Get<double>("ts3.c5.to_client");
            c5_1.Add(ComplexFromScalars(c5_1_1));
            return c5_1;
        }
        set {
            double[] c5_2_1 = TestVectors.Get<double>("ts3.c5.to_service");
            ca(value[0], ComplexFromScalars(c5_2_1));
        }
    }
    public override CSingle c7
    {
        get {
            return new CSingle(-5.527021e-18f, -9.848457e+03f);
        }
        set {
            var c7_2 = new CSingle(9.303345e-12f, -3.865684e-05f);
            RRAssert.AreEqual((object)value, (object)c7_2);
        }
    }
    public override CSingle[] c8
    {
        get {
            float[] c8_1_1 = TestVectors.Get<float>("ts3.c8.to_client");
            return ComplexFromScalars(c8_1_1);
        }
        set {
            float[] c8_2_1 = TestVectors.Get<float>("ts3.c8.to_service");
            ca(value, ComplexFromScalars(c8_2_1));
        }
    }
    public override MultiDimArray c9
    {
        get {
            uint[] c9_1_1 = TestVectors.Get<uint>("ts3.c9.to_client.dims");
            float[] c9_1_2 = TestVectors.Get<float>("ts3.c9.to_client");
            return new MultiDimArray(c9_1_1, ComplexFromScalars(c9_1_2));
        }
        set {
            uint[] c9_2_1 = TestVectors.Get<uint>("ts3.c9.to_service.dims");
            float[] c9_2_2 = TestVectors.Get<float>("ts3.c9.to_service");
            ca(value.Dims, c9_2_1);
            ca((CSingle[])value.Array_, ComplexFromScalars(c9_2_2));
        }
    }

    public override bool b1
    {
        get {
            return true;
        }
        set {
            RRAssert.AreEqual(value, true);
        }
    }

    public override bool[] b2
    {
        get {
            return new bool[] { true, false, true, true, false, true, false };
        }
        set {
            ca(value, new bool[] { true, false, false, true, true, true, false, true });
        }
    }

    public override MultiDimArray b3
    {
        get {
            return new MultiDimArray(new uint[] { 2, 2 }, new bool[] { false, true, true, false });
        }
        set {
            ca(value.Dims, new uint[] { 2, 1 });
            ca((bool[])value.Array_, new bool[] { true, false });
        }
    }

    public override List<bool> b4
    {
        get {
            var o = new List<bool>();
            o.Add(true);
            return o;
        }
        set {
            RRAssert.AreEqual(value[0], true);
        }
    }

    public override List<bool[]> b5
    {
        get {
            var o = new List<bool[]>();
            o.Add(new bool[] { false, true, false, false });
            return o;
        }
        set {
            ca(value[0], new bool[] { true, false });
        }
    }

    public override List<MultiDimArray> b6
    {
        get {
            var o = new List<MultiDimArray>();
            o.Add(new MultiDimArray(new uint[] { 2, 2 }, new bool[] { false, true, true, false }));
            return o;
        }
        set {
            ca(value[0].Dims, new uint[] { 2, 1 });
            ca((bool[])value[0].Array_, new bool[] { true, false });
        }
    }

    public override ArrayMemory<CDouble> c_m1 { get; } = new ArrayMemory<CDouble>(new CDouble[512]);

    public override MultiDimArrayMemory<CDouble> c_m2 {
        get;
    } = new MultiDimArrayMemory<CDouble>(new MultiDimArray(new uint[] { 10, 10 }, new CDouble[100]));

    public override obj5 get_nolock_test()
    {
        return o5;
    }

    obj5_impl o5 = new obj5_impl();

    public override ArrayMemory<bool> c_m5 { get; } = new ArrayMemory<bool>(new bool[512]);
    public override MultiDimArrayMemory<bool> c_m6 {
        get;
    } = new MultiDimArrayMemory<bool>(new MultiDimArray(new uint[] { 10, 10 }, new bool[100]));

    public override void test_exception_params1()
    {
        var params_ = new Dictionary<string, object>();
        params_.Add("param1", 10);
        params_.Add("param2", "20");
        var exp = new InvalidOperationException("test error");
        exp.Data.Add("ErrorSubName", "my_error");
        exp.Data.Add("ErrorParam", params_);
        throw exp;
    }

    public override void test_exception_params2()
    {
        var params_ = new Dictionary<string, object>();
        params_.Add("param1", 30);
        params_.Add("param2", "40");
        throw new com.robotraconteur.testing.TestService3.test_exception4("test error2", "my_error2", params_);
    }
}

class func4_gen : SyncGenerator1<byte[], byte[]>
{
    byte j = 0;
    bool aborted = false;

    public override void Abort()
    {
        aborted = true;
    }

    public override void Close()
    {
        j = 255;
    }

    public override byte[] Next(byte[] param)
    {
        if (aborted)
        {
            throw new OperationAbortedException("");
        }
        if (j >= 8)
        {
            throw new StopIterationException("");
        }

        var a = new byte[param.Length];
        for (int i = 0; i < param.Length; i++)
        {
            a[i] = (byte)(param[i] + j);
        }
        j++;
        return a;
    }
}

class obj4_impl : obj4
{
    public virtual string s_ind { get; set; }
    public virtual int i_ind { get; set; }
    public virtual string data { get; set; }
    public com.robotraconteur.testing.TestService1.sub3 get_o3_1(string ind) => null;
}

class obj5_impl : obj5_default_impl
{
    ArrayMemory<int> _m1 = new ArrayMemory<int>(new int[100]);
    ArrayMemory<int> _m2 = new ArrayMemory<int>(new int[100]);
    ArrayMemory<int> _m3 = new ArrayMemory<int>(new int[100]);

    public override int f1()
    {
        return 10;
    }

    public override int f2()
    {
        return 11;
    }

    public override ArrayMemory<int> m1
    {
        get {
            return _m1;
        }
    }

    public override ArrayMemory<int> m2
    {
        get {
            return _m2;
        }
    }

    public override ArrayMemory<int> m3
    {
        get {
            return _m3;
        }
    }
}

class ServiceTest2_test_sequence_gen
{
    private uint counter;
    private uint step;
    static readonly double[] double_constants = TestVectors.Get<double>("seq.double_constants");
    static readonly uint[] uint32_constants = TestVectors.Get<uint>("seq.uint32_constants");

    public ServiceTest2_test_sequence_gen(uint seed)
    {
        counter = seed % 7823;
        step = seed % 17 + 7;
    }

    public double get_double()
    {
        counter += step;
        return double_constants[counter % double_constants.Length];
    }

    public uint get_uint32()
    {
        counter += step;
        return uint32_constants[counter % uint32_constants.Length];
    }

    public sbyte get_int8()
    {
        counter += step;
        return (sbyte)(((int)(counter % 0xFF)) - 128);
    }
}

static class ServiceTest2_pod
{
    private static double[] create_double_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        var o = new double[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_double();
        }
        return o;
    }

    private static void verify_double_array(ServiceTest2_test_sequence_gen gen, double[] v, int len)
    {
        RRAssert.AreEqual(v.Length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.AreEqual(v[i], gen.get_double());
        }
    }

    private static uint[] create_uint32_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        var o = new uint[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_uint32();
        }
        return o;
    }

    private static void verify_uint32_array(ServiceTest2_test_sequence_gen gen, uint[] v, int len)
    {
        RRAssert.AreEqual(v.Length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.AreEqual(v[i], gen.get_uint32());
        }
    }

    private static sbyte[] create_int8_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        var o = new sbyte[len];
        for (int i = 0; i < len; i++)
        {
            o[i] = gen.get_int8();
        }
        return o;
    }

    private static void verify_int8_array(ServiceTest2_test_sequence_gen gen, sbyte[] v, int len)
    {
        RRAssert.AreEqual(v.Length, len);
        for (int i = 0; i < len; i++)
        {
            RRAssert.AreEqual(v[i], gen.get_int8());
        }
    }

    public static void fill_testpod1(ref testpod1 s, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        s.d1 = gen.get_double();
        s.d2 = create_double_array(gen, 6);
        s.d3 = create_double_array(gen, (int)(gen.get_uint32() % 6));
        s.d4 = create_double_array(gen, 9);

        fill_testpod2(ref s.s1, gen.get_uint32());
        s.s2 = create_testpod2_array(gen, 8);
        s.s3 = create_testpod2_array(gen, (int)(gen.get_uint32() % 9));
        s.s4 = create_testpod2_array(gen, 8);

        fill_transform(ref s.t1, gen.get_uint32());

        s.t2 = new transform[4];
        for (int i = 0; i < 4; i++)
            fill_transform(ref s.t2[i], gen.get_uint32());

        int t3_len = (int)(gen.get_uint32() % 15);
        s.t3 = new transform[(t3_len)];
        for (int i = 0; i < t3_len; i++)
            fill_transform(ref s.t3[i], gen.get_uint32());

        s.t4 = new transform[8];
        for (int i = 0; i < 8; i++)
            fill_transform(ref s.t4[i], gen.get_uint32());
    }

    public static void verify_testpod1(ref testpod1 s, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.AreEqual(s.d1, gen.get_double());
        verify_double_array(gen, s.d2, 6);
        verify_double_array(gen, s.d3, (int)(gen.get_uint32() % 6));

        verify_double_array(gen, (double[])s.d4, 9);
        verify_testpod2(ref s.s1, gen.get_uint32());
        verify_testpod2_array(gen, s.s2, 8);
        verify_testpod2_array(gen, s.s3, (int)(gen.get_uint32() % 9));
        verify_testpod2_array(gen, (testpod2[])s.s4, 8);

        verify_transform(ref s.t1, gen.get_uint32());

        for (int i = 0; i < 4; i++)
            verify_transform(ref s.t2[i], gen.get_uint32());

        int t3_len = (int)(gen.get_uint32() % 15);
        RRAssert.AreEqual(s.t3.Length, t3_len);
        for (int i = 0; i < t3_len; i++)
            verify_transform(ref s.t3[i], gen.get_uint32());

        for (int i = 0; i < 8; i++)
            verify_transform(ref s.t4[i], gen.get_uint32());
    }

    public static void fill_testpod2(ref testpod2 s, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        s.i1 = gen.get_int8();
        s.i2 = create_int8_array(gen, 15);
        s.i3 = create_int8_array(gen, (int)(gen.get_uint32() % 15));
    }

    public static void verify_testpod2(ref testpod2 s, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.AreEqual(s.i1, gen.get_int8());
        verify_int8_array(gen, s.i2, 15);
        verify_int8_array(gen, s.i3, (int)(gen.get_uint32() % 15));
    }

    public static testpod1[] create_testpod1_array(int len, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var o = new testpod1[len];
        for (int i = 0; i < len; i++)
        {
            fill_testpod1(ref o[i], gen.get_uint32());
        }
        return o;
    }

    public static void verify_testpod1_array(testpod1[] v, int len, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.AreEqual(v.Length, len);
        for (int i = 0; i < len; i++)
        {
            verify_testpod1(ref v[i], gen.get_uint32());
        }
    }

    public static PodMultiDimArray create_testpod1_multidimarray(uint m, uint n, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var o = new testpod1[m * n];
        for (int i = 0; i < m * n; i++)
        {
            fill_testpod1(ref o[i], gen.get_uint32());
        }
        return new PodMultiDimArray(new uint[] { m, n }, o);
    }

    public static void verify_testpod1_multidimarray(PodMultiDimArray v, uint m, uint n, uint seed)
    {
        RRAssert.IsTrue(v.Dims.SequenceEqual(new uint[] { m, n }));

        verify_testpod1_array((testpod1[])v.pod_array, (int)(m * n), seed);
    }

    public static void verify_testpod1_multidimarray(object v, uint m, uint n, uint seed)
    {
        if ((v is Array) && n == 1)
        {
            verify_testpod1_array((testpod1[])(v), (int)m, seed);
        }
        else
        {
            var v2 = (PodMultiDimArray)v;
            RRAssert.IsTrue(v2.Dims.SequenceEqual(new uint[] { m, n }));

            verify_testpod1_array((testpod1[])(v2.pod_array), (int)(m * n), seed);
        }
    }

    public static testpod2[] create_testpod2_array(ServiceTest2_test_sequence_gen gen, int len)
    {
        var o = new testpod2[len];
        for (int i = 0; i < len; i++)
        {
            fill_testpod2(ref o[i], gen.get_uint32());
        }
        return o;
    }

    public static void verify_testpod2_array(ServiceTest2_test_sequence_gen gen, testpod2[] v, int len)
    {
        RRAssert.AreEqual(v.Length, len);
        for (int i = 0; i < len; i++)
        {
            verify_testpod2(ref v[i], gen.get_uint32());
        }
    }

    public static teststruct3 fill_teststruct3(uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var o = new teststruct3();
        fill_testpod1(ref o.s1, gen.get_uint32());
        var s2_seed = gen.get_uint32();
        o.s2 = create_testpod1_array((int)(s2_seed % 17), s2_seed);
        o.s3 = create_testpod1_array(11, gen.get_uint32());
        var s4_seed = gen.get_uint32();
        o.s4 = create_testpod1_array((int)(s4_seed % 16), s4_seed);
        o.s5 = create_testpod1_multidimarray(3, 3, gen.get_uint32());
        var s6_seed = gen.get_uint32();
        o.s6 = create_testpod1_multidimarray((uint)(s6_seed % 6), (uint)(s6_seed % 3), s6_seed);
        o.s7 = new List<testpod1>();
        var s7_1 = new testpod1();
        fill_testpod1(ref s7_1, gen.get_uint32());
        o.s7.Add(s7_1);

        o.s8 = new List<testpod1[]>();
        o.s8.Add(create_testpod1_array(2, gen.get_uint32()));
        o.s8.Add(create_testpod1_array(4, gen.get_uint32()));

        o.s9 = new List<PodMultiDimArray>();
        o.s9.Add(create_testpod1_multidimarray(2, 3, gen.get_uint32()));
        o.s9.Add(create_testpod1_multidimarray(4, 5, gen.get_uint32()));

        var s10 = new testpod1();
        fill_testpod1(ref s10, gen.get_uint32());
        o.s10 = new testpod1[] { s10 };

        o.s11 = create_testpod1_array(3, gen.get_uint32());
        o.s12 = create_testpod1_multidimarray(2, 2, gen.get_uint32());

        var s13 = new testpod1();
        fill_testpod1(ref s13, gen.get_uint32());
        o.s13 = new List<object>(new object[] { new testpod1[] { s13 } });

        var s14 = new List<object>();
        s14.Add(create_testpod1_array(3, gen.get_uint32()));
        s14.Add(create_testpod1_array(5, gen.get_uint32()));
        o.s14 = s14;

        var s15 = new List<object>();
        s15.Add(create_testpod1_multidimarray(7, 2, gen.get_uint32()));
        s15.Add(create_testpod1_multidimarray(5, 1, gen.get_uint32()));
        o.s15 = s15;

        fill_transform(ref o.t1, gen.get_uint32());

        o.t2 = fill_transform_array(4, gen.get_uint32());
        o.t3 = fill_transform_multidimarray(2, 4, gen.get_uint32());

        o.t4 = fill_transform_array(10, gen.get_uint32());
        o.t5 = fill_transform_multidimarray(6, 5, gen.get_uint32());

        o.t6 = new List<transform>();
        var t6_1 = new transform();
        fill_transform(ref t6_1, gen.get_uint32());
        o.t6.Add(t6_1);

        o.t7 = new List<transform[]>();
        o.t7.Add(fill_transform_array(4, gen.get_uint32()));
        o.t7.Add(fill_transform_array(4, gen.get_uint32()));

        o.t8 = new List<NamedMultiDimArray>();
        o.t8.Add(fill_transform_multidimarray(2, 4, gen.get_uint32()));
        o.t8.Add(fill_transform_multidimarray(2, 4, gen.get_uint32()));

        var t9 = new transform();
        fill_transform(ref t9, gen.get_uint32());
        o.t9 = new List<object>(new object[] { new transform[] { t9 } });

        var t10 = new List<object>();
        t10.Add(fill_transform_array(3, gen.get_uint32()));
        t10.Add(fill_transform_array(5, gen.get_uint32()));
        o.t10 = t10;

        var t11 = new List<object>();
        t11.Add(fill_transform_multidimarray(7, 2, gen.get_uint32()));
        t11.Add(fill_transform_multidimarray(5, 1, gen.get_uint32()));
        o.t11 = t11;

        return o;
    }

    public static void verify_teststruct3(teststruct3 v, uint seed)
    {
        RRAssert.AreNotEqual(v, null);
        var gen = new ServiceTest2_test_sequence_gen(seed);

        verify_testpod1(ref v.s1, gen.get_uint32());
        var s2_seed = gen.get_uint32();
        verify_testpod1_array(v.s2, (int)(s2_seed % 17), s2_seed);
        verify_testpod1_array(v.s3, 11, gen.get_uint32());
        var s4_seed = gen.get_uint32();
        verify_testpod1_array(v.s4, (int)(s4_seed % 16), s4_seed);
        verify_testpod1_multidimarray(v.s5, 3, 3, gen.get_uint32());
        var s6_seed = gen.get_uint32();
        verify_testpod1_multidimarray(v.s6, (uint)(s6_seed % 6), (uint)(s6_seed % 3), s6_seed);

        RRAssert.AreNotEqual(v.s7, null);
        RRAssert.AreEqual(v.s7.Count, 1);
        var s7_0 = v.s7[0];
        verify_testpod1(ref s7_0, gen.get_uint32());

        RRAssert.AreNotEqual(v.s8, null);
        RRAssert.AreEqual(v.s8.Count, 2);
        verify_testpod1_array(v.s8[0], 2, gen.get_uint32());
        verify_testpod1_array(v.s8[1], 4, gen.get_uint32());

        RRAssert.AreNotEqual(v.s9, null);
        RRAssert.AreEqual(v.s9.Count, 2);
        verify_testpod1_multidimarray(v.s9[0], 2, 3, gen.get_uint32());
        verify_testpod1_multidimarray(v.s9[1], 4, 5, gen.get_uint32());

        var s10 = ((testpod1[])v.s10)[0];
        verify_testpod1(ref s10, gen.get_uint32());

        verify_testpod1_array((testpod1[])v.s11, 3, gen.get_uint32());
        verify_testpod1_multidimarray(v.s12, 2, 2, gen.get_uint32());

        RRAssert.AreNotEqual(v.s13, null);
        var s13 = (testpod1[])((List<object>)v.s13)[0];
        verify_testpod1(ref s13[0], gen.get_uint32());

        RRAssert.AreNotEqual(v.s14, null);
        var v14 = (List<object>)v.s14;
        RRAssert.AreEqual(v14.Count, 2);
        verify_testpod1_array((testpod1[])v14[0], 3, gen.get_uint32());
        verify_testpod1_array((testpod1[])v14[1], 5, gen.get_uint32());

        RRAssert.AreNotEqual(v.s15, null);
        var v15 = (List<object>)v.s15;
        RRAssert.AreEqual(v15.Count, 2);
        verify_testpod1_multidimarray(v15[0], 7, 2, gen.get_uint32());
        verify_testpod1_multidimarray(v15[1], 5, 1, gen.get_uint32());

        verify_transform(ref v.t1, gen.get_uint32());

        verify_transform_array(v.t2, 4, gen.get_uint32());
        verify_transform_multidimarray(v.t3, 2, 4, gen.get_uint32());

        verify_transform_array((transform[])(v.t4), 10, gen.get_uint32());
        verify_transform_multidimarray((NamedMultiDimArray)(v.t5), 6, 5, gen.get_uint32());

        RRAssert.AreNotEqual(v.t6, null);
        RRAssert.AreEqual(v.t6.Count, 1);
        var t6_0 = v.t6[0];
        verify_transform(ref t6_0, gen.get_uint32());

        RRAssert.AreNotEqual(v.t7, null);
        RRAssert.AreEqual(v.t7.Count, 2);
        verify_transform_array(v.t7[0], 4, gen.get_uint32());
        verify_transform_array(v.t7[1], 4, gen.get_uint32());

        RRAssert.AreNotEqual(v.t8, null);
        RRAssert.AreEqual(v.t8.Count, 2);
        verify_transform_multidimarray(v.t8[0], 2, 4, gen.get_uint32());
        verify_transform_multidimarray(v.t8[1], 2, 4, gen.get_uint32());

        RRAssert.AreNotEqual(v.t9, null);
        var t9 = (transform[])((List<object>)v.t9)[0];
        verify_transform(ref t9[0], gen.get_uint32());

        RRAssert.AreNotEqual(v.t10, null);
        var t10 = (List<object>)v.t10;
        RRAssert.AreEqual(t10.Count, 2);
        verify_transform_array((transform[])t10[0], 3, gen.get_uint32());
        verify_transform_array((transform[])t10[1], 5, gen.get_uint32());

        RRAssert.AreNotEqual(v.t11, null);
        var t11 = (List<object>)v.t11;
        RRAssert.AreEqual(t11.Count, 2);
        verify_transform_multidimarray((NamedMultiDimArray)t11[0], 7, 2, gen.get_uint32());
        verify_transform_multidimarray((NamedMultiDimArray)t11[1], 5, 1, gen.get_uint32());
    }

    public static void fill_transform(ref transform t, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var a = new double[7];
        for (int i = 0; i < 7; i++)
            a[i] = gen.get_double();
        var a2 = new ArraySegment<double>(a);
        t.AssignFromNumericArray(ref a2);
    }

    public static void verify_transform(ref transform t, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var a = t.GetNumericArray();
        for (int i = 0; i < 7; i++)
            RRAssert.AreEqual(a[i], gen.get_double());
    }

    public static transform[] fill_transform_array(int len, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        var o = new transform[len];
        for (int i = 0; i < len; i++)
            fill_transform(ref o[i], gen.get_uint32());
        return o;
    }

    public static void verify_transform_array(transform[] t, int len, uint seed)
    {
        var gen = new ServiceTest2_test_sequence_gen(seed);
        RRAssert.AreEqual(t.Length, len);
        for (int i = 0; i < len; i++)
            verify_transform(ref t[i], gen.get_uint32());
    }

    public static NamedMultiDimArray fill_transform_multidimarray(uint m, uint n, uint seed)
    {
        return new NamedMultiDimArray(new uint[] { m, n }, fill_transform_array((int)(m * n), seed));
    }

    public static void verify_transform_multidimarray(NamedMultiDimArray a, uint m, uint n, uint seed)
    {
        RRAssert.IsTrue(a.Dims.SequenceEqual(new uint[] { m, n }));
        verify_transform_array((transform[])a.namedarray_array, (int)(m * n), seed);
    }
}

}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using com.robotraconteur.testing.TestService1;
using com.robotraconteur.testing.TestService2;
using System.Threading;

// cSpell: ignore mydat, lstruct
namespace RobotRaconteurNETTest
{

public class RobotRaconteurTestServiceSupport
{
    public RobotRaconteurTest_testroot testservice;
    public RobotRaconteurTest_testroot testservice_auth;

    public void RegisterServices(TcpTransport t)
    {
        testservice = new RobotRaconteurTest_testroot(t);
        testservice_auth = new RobotRaconteurTest_testroot(t);

        ServerContext c = RobotRaconteurNode.s.RegisterService("RobotRaconteurTestService",
                                                               "com.robotraconteur.testing.TestService1", testservice);
        c.AddServerServiceListener(service_listener);

        string authdata =
            "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride";
        PasswordFileUserAuthenticator p = new PasswordFileUserAuthenticator(authdata);
        Dictionary<string, string> policies = new Dictionary<string, string>();
        policies.Add("requirevaliduser", "true");
        policies.Add("allowobjectlock", "true");
        ServiceSecurityPolicy s = new ServiceSecurityPolicy(p, policies);

        RobotRaconteurNode.s.RegisterService("RobotRaconteurTestService_auth",
                                             "com.robotraconteur.testing.TestService1", testservice_auth, s);
    }

    public void service_listener(ServerContext service, ServerServiceListenerEventType ev, object parameter)
    {
        Console.WriteLine("Server event: " + ev + " " + parameter.ToString());
    }

    public void UnregisterServices()
    {
        RobotRaconteurNode.s.CloseService("RobotRaconteurTestService");
        RobotRaconteurNode.s.CloseService("RobotRaconteurTestService_auth");
    }
}

public class RobotRaconteurTest_testroot : com.robotraconteur.testing.TestService1.testroot
{
    public RobotRaconteurTest_testroot(TcpTransport t)
    {
        tcptransport = t;
    }

    public double d1
    {
        get {
            return 12.345;
        }
        set {
            RRAssert.AreEqual(value, 3.456);
        }
    }

    public double[] d2
    {
        get {
            return TestVectors.Get<double>("ts1.d2.to_client");
        }
        set {
            double[] testval = TestVectors.Get<double>("ts1.d2.to_service");
            ca<double>(testval, value);
        }
    }

    public void ca<T>(T[] v1, T[] v2)
        where T : IComparable, IComparable<T>
    {
        RRAssert.AreEqual(v1.Length, v2.Length);

        for (int i = 0; i < v1.Length; i++)
        {
            RRAssert.AreEqual<T>(v1[i], v2[i]);
        }
    }

    public double[] d3
    {
        get {
            return TestVectors.Get<double>("ts1.d3.to_client");
        }
        set {
            double[] testval = TestVectors.Get<double>("ts1.d3.to_service");
            ca<double>(testval, value);
        }
    }

    public double[] d4
    {
        get {
            return TestVectors.Get<double>("ts1.d4.to_client");
        }
        set {
            double[] testval = TestVectors.Get<double>("ts1.d4.to_service");
            ca<double>(value, testval);
        }
    }

    public MultiDimArray d5
    {
        get {
            double[] r = TestVectors.Get<double>("ts1.d5.to_client");

            uint[] dims = TestVectors.Get<uint>("ts1.d5.to_client.dims");
            return new MultiDimArray(dims, r);
        }
        set {
            double[] r = TestVectors.Get<double>("ts1.d5.to_service");

            uint[] dims = TestVectors.Get<uint>("ts1.d5.to_service.dims");

            ca<uint>(dims, value.Dims);
            ca<double>(r, (double[])value.Array_);
        }
    }

    public MultiDimArray d6
    {
        get {
            double[] r = TestVectors.Get<double>("ts1.d6.to_client");

            uint[] dims = TestVectors.Get<uint>("ts1.d6.to_client.dims");
            return new MultiDimArray(dims, r);
        }
        set {
            double[] r = TestVectors.Get<double>("ts1.d6.to_service");

            uint[] dims = TestVectors.Get<uint>("ts1.d6.to_service.dims");

            ca<uint>(dims, value.Dims);
            ca<double>(r, (double[])value.Array_);
        }
    }

    public float s1
    {
        get {
            return 7.8573f;
        }
        set {
            RRAssert.AreEqual(value, 3847.9283f);
        }
    }

    public float[] s2
    {
        get {
            return TestVectors.Get<float>("ts1.s2.to_client");
        }
        set {
            float[] testval = TestVectors.Get<float>("ts1.s2.to_service");
            ca<float>(value, testval);
        }
    }

    public sbyte i8_1
    {
        get {
            return -66;
        }
        set {
            RRAssert.AreEqual(value, 45);
        }
    }

    public sbyte[] i8_2
    {
        get {
            return TestVectors.Get<sbyte>("ts1.i8_2.to_client");
        }
        set {
            sbyte[] testval = TestVectors.Get<sbyte>("ts1.i8_2.to_service");
            ca<sbyte>(value, testval);
        }
    }

    public byte u8_1
    {
        get {
            return 222;
        }
        set {
            RRAssert.AreEqual(value, 232);
        }
    }

    public byte[] u8_2
    {
        get {
            return TestVectors.Get<byte>("ts1.u8_2.to_client");
        }
        set {
            byte[] testval = TestVectors.Get<byte>("ts1.u8_2.to_service");
            ca<byte>(value, testval);
        }
    }

    public MultiDimArray u8_3
    {
        get {
            byte[] r = TestVectors.Get<byte>("ts1.u8_3.to_client");
            uint[] dims = TestVectors.Get<uint>("ts1.u8_3.to_client.dims");
            return new MultiDimArray(dims, r);
        }
        set {
            byte[] testval = TestVectors.Get<byte>("ts1.u8_3.to_service");
            uint[] testdims = TestVectors.Get<uint>("ts1.u8_3.to_service.dims");
            ca<uint>(value.Dims, testdims);
            ca<byte>((byte[])value.Array_, testval);
        }
    }
    public short i16_1
    {
        get {
            return -13428;
        }
        set {
            RRAssert.AreEqual(value, 2387);
        }
    }
    public short[] i16_2
    {
        get {
            return TestVectors.Get<short>("ts1.i16_2.to_client");
        }
        set {
            short[] testval = TestVectors.Get<short>("ts1.i16_2.to_service");
            ca<short>(value, testval);
        }
    }

    public ushort u16_1
    {
        get {
            return 60981;
        }
        set {
            RRAssert.AreEqual(value, 54732);
        }
    }

    public ushort[] u16_2
    {
        get {
            return TestVectors.Get<ushort>("ts1.u16_2.to_client");
        }
        set {
            ushort[] testval = TestVectors.Get<ushort>("ts1.u16_2.to_service");
            ca<ushort>(value, testval);
        }
    }

    public int i32_1
    {
        get {
            return 898734;
        }
        set {
            RRAssert.AreEqual(value, -9837284);
        }
    }

    public int[] i32_2
    {
        get {
            return TestVectors.Get<int>("ts1.i32_2.to_client");
        }
        set {
            int[] testval = TestVectors.Get<int>("ts1.i32_2.to_service");
            ca<int>(value, testval);
        }
    }

    public int[] i32_huge
    {
        get {
            var o = new int[2621440];
            for (int i = 0; i < 2621440; i++)
            {
                o[i] = i;
            }
            return o;
        }
        set {
            RRAssert.AreEqual(value.Length, 2621440);

            for (int i = 0; i < 2621440; i++)
            {
                RRAssert.AreEqual(value[i], -i);
            }
        }
    }

    public uint u32_1
    {
        get {
            return 547919123;
        }
        set {
            RRAssert.AreEqual<uint>(value, 1550099195);
        }
    }

    public uint[] u32_2
    {
        get {
            return TestVectors.Get<uint>("ts1.u32_2.to_client");
        }
        set {
            uint[] testval = TestVectors.Get<uint>("ts1.u32_2.to_service");
            ca<uint>(testval, value);
        }
    }
    public long i64_1
    {
        get {
            return -1357833931563696072;
        }
        set {
            RRAssert.AreEqual(value, 8621740821050813024);
        }
    }
    public long[] i64_2
    {
        get {
            return TestVectors.Get<long>("ts1.i64_2.to_client");
        }
        set {
            long[] testval = TestVectors.Get<long>("ts1.i64_2.to_service");
        }
    }

    public ulong u64_1
    {
        get {
            return 13389861970863644378;
        }
        set {
            RRAssert.AreEqual<ulong>(value, 1465640522145789825);
        }
    }

    public ulong[] u64_2
    {
        get {
            return TestVectors.Get<ulong>("ts1.u64_2.to_client");
        }
        set {
            ulong[] testval = TestVectors.Get<ulong>("ts1.u64_2.to_service");
            ca<ulong>(value, testval);
        }
    }
    public string str1
    {
        get {
            return "Hello Client!";
        }
        set {
            RRAssert.AreEqual(value, "Hello Server!");
        }
    }

    public teststruct1 struct1
    {
        get {
            teststruct1 s = new teststruct1();
            s.dat1 = TestVectors.Get<double>("ts1.struct1.dat1.to_client");
            s.str2 = "Hello world!";

            Dictionary<int, string> vec3 = new Dictionary<int, string>();
            vec3.Add(1, "Hello Client!");
            vec3.Add(2, "Hello Client, again");
            vec3.Add(4372, "This is yet another test string");
            s.vec3 = vec3;

            Dictionary<string, string> dict4 = new Dictionary<string, string>();
            dict4.Add("teststring1", "Hello Client!");
            dict4.Add("teststring2", "Hello Client, again");
            dict4.Add("anotherstr", "This is yet another test string");
            s.dict4 = dict4;

            List<string> list5 = new List<string>();
            list5.Add("Hello Client!");
            list5.Add("Hello Client, again");
            list5.Add("This is yet another test string");
            s.list5 = list5;

            teststruct2 s2 = new teststruct2();
            s2.mydat = TestVectors.Get<double>("ts1.struct1.struct1_mydat.to_client");
            s.struct1 = s2;

            Dictionary<string, teststruct2> d2 = new Dictionary<string, teststruct2>();
            teststruct2 d2_1 = new teststruct2();
            d2_1.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_client");
            d2.Add("test1", d2_1);
            teststruct2 d2_2 = new teststruct2();
            d2_2.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_3.to_client");
            d2.Add("anothertest", d2_2);

            s.dstruct2 = d2;

            List<teststruct2> l3 = new List<teststruct2>();
            teststruct2 l3_1 = new teststruct2();
            l3_1.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_client");
            l3.Add(l3_1);
            teststruct2 l3_2 = new teststruct2();
            l3_2.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_3.to_client");
            l3.Add(l3_2);

            s.lstruct3 = l3;

            double[] md = TestVectors.Get<double>("ts1.struct1.multidimarray.to_client");
            s.multidimarray = new MultiDimArray(TestVectors.Get<uint>("ts1.struct1.multidimarray.to_client.dims"), md);
            s.var3 = "This is a vartype string";
            return s;
        }
        set {
            ca<double>(value.dat1, TestVectors.Get<double>("ts1.struct1.dat1.to_service"));
            RRAssert.AreEqual(value.str2, "Hello world 2!");
            RRAssert.AreEqual(value.vec3.Count, 4);
            RRAssert.AreEqual(value.vec3[10], "Hello Server!");
            RRAssert.AreEqual(value.vec3[11], "Hello Server, again");
            RRAssert.AreEqual(value.vec3[46372], "Test string!");
            RRAssert.AreEqual(value.vec3[46373], "Test string again");
            RRAssert.AreEqual(value.dict4.Count, 2);

            RRAssert.AreEqual(value.dict4["cteststring1"], "Hello Server!");

            RRAssert.AreEqual(value.dict4["cteststring2"], "Hello Server, again");

            RRAssert.AreEqual(value.list5.Count, 2);

            RRAssert.AreEqual(value.list5[0], "Hello Server!");

            RRAssert.AreEqual(value.list5[1], "Hello Server, again");

            ca<double>(value.struct1.mydat, TestVectors.Get<double>("ts1.struct1.struct1_mydat.to_service"));
            ca<double>(value.dstruct2["ctest1"].mydat, TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_service"));
            ca<double>(value.dstruct2["anothertest"].mydat,
                       TestVectors.Get<double>("ts1.struct1.dstruct2_2.to_service"));

            ca<double>(value.lstruct3[0].mydat, TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_service"));
            ca<double>(value.lstruct3[1].mydat, TestVectors.Get<double>("ts1.struct1.dstruct2_2.to_service"));

            ca<uint>(value.multidimarray.Dims, TestVectors.Get<uint>("ts1.struct1.multidimarray.to_service.dims"));
            ca<double>((double[])value.multidimarray.Array_,
                       TestVectors.Get<double>("ts1.struct1.multidimarray.to_service"));

            ca((double[])value.var3, TestVectors.Get<double>("ts1.struct1.var3.to_service"));
        }
    }

    public teststruct2 struct2
    {
        get {
            teststruct2 a = new teststruct2();
            a.mydat = TestVectors.Get<double>("ts1.struct2.to_client");
            return a;
        }

        set {
            ca<double>(value.mydat, TestVectors.Get<double>("ts1.struct2.to_service"));
        }
    }

    public Dictionary<int, double> is_d1
    {
        get {
            Dictionary<int, double> o = new Dictionary<int, double>();
            o.Add(9285, 1.643392e-01);
            o.Add(74822, 1.537133e+09);
            o.Add(4, 1.369505e-03);
            return o;
        }

        set {
            RRAssert.AreEqual(value.Count, 3);
            RRAssert.AreEqual(value[928], 4.074501e-07);
            RRAssert.AreEqual(value[394820], -4.535303e+05);
            RRAssert.AreEqual(value[623], -2.956241e-20);
        }
    }
    public Dictionary<string, double> is_d2
    {
        get {
            Dictionary<string, double> o = new Dictionary<string, double>();
            o.Add("testval1", -1.079664e+16);
            o.Add("testval2", 2.224846e+00);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 2);
            RRAssert.AreEqual(value["testval3"], 5.242474e+10);
            RRAssert.AreEqual(value["testval4"], 2.208636e+08);
        }
    }

    public Dictionary<int, double[]> is_d3
    {
        get {
            Dictionary<int, double[]> o = new Dictionary<int, double[]>();
            o.Add(12, TestVectors.Get<double>("ts1.is_d3.to_client.a"));
            o.Add(832, TestVectors.Get<double>("ts1.is_d3.to_client.b"));
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 2);

            ca<double>(value[47], TestVectors.Get<double>("ts1.is_d3.to_service.a"));
            ca<double>(value[324], TestVectors.Get<double>("ts1.is_d3.to_service.b"));
        }
    }

    public Dictionary<string, double[]> is_d4
    {
        get {
            Dictionary<string, double[]> o = new Dictionary<string, double[]>();
            o.Add("testval1", TestVectors.Get<double>("ts1.is_d4.to_client.a"));
            o.Add("testval2", TestVectors.Get<double>("ts1.is_d4.to_client.b"));
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 2);
            ca<double>(value["testval3"], TestVectors.Get<double>("ts1.is_d4.to_service.a"));
            ca<double>(value["testval4"], TestVectors.Get<double>("ts1.is_d4.to_service.b"));
        }
    }

    public Dictionary<int, MultiDimArray> is_d5
    {
        get {
            MultiDimArray a = new MultiDimArray(TestVectors.Get<uint>("ts1.is_d5.to_client.dims"),
                                                TestVectors.Get<double>("ts1.is_d5.to_client"));
            Dictionary<int, MultiDimArray> o = new Dictionary<int, MultiDimArray>();
            o.Add(564, a);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            MultiDimArray a = value[328];
            ca<uint>(a.Dims, TestVectors.Get<uint>("ts1.is_d5.to_service.dims"));
            ca<double>((double[])a.Array_, TestVectors.Get<double>("ts1.is_d5.to_service"));
        }
    }

    public Dictionary<string, MultiDimArray> is_d6
    {
        get {
            MultiDimArray a = new MultiDimArray(TestVectors.Get<uint>("ts1.is_d6.to_client.dims"),
                                                TestVectors.Get<double>("ts1.is_d6.to_client"));
            Dictionary<string, MultiDimArray> o = new Dictionary<string, MultiDimArray>();
            o.Add("testval1", a);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            MultiDimArray a = value["testval2"];
            ca<uint>(a.Dims, TestVectors.Get<uint>("ts1.is_d6.to_service.dims"));
            ca<double>((double[])a.Array_, TestVectors.Get<double>("ts1.is_d6.to_service"));
        }
    }

    public Dictionary<int, string> is_str1
    {
        get {
            Dictionary<int, string> o = new Dictionary<int, string>();
            o.Add(23, "Hello server");
            return o;
        }

        set {
            RRAssert.AreEqual(value.Count, 1);
            RRAssert.AreEqual(value[24], "Hello client");
        }
    }

    public Dictionary<string, string> is_str2
    {
        get {
            Dictionary<string, string> o = new Dictionary<string, string>();
            o.Add("testval1", "Hello server");
            return o;
        }

        set {
            RRAssert.AreEqual(value.Count, 1);
            RRAssert.AreEqual(value["testval2"], "Hello client");
        }
    }

    public Dictionary<int, teststruct2> is_struct1
    {
        get {
            Dictionary<int, teststruct2> o = new Dictionary<int, teststruct2>();
            teststruct2 s = new teststruct2();
            s.mydat = TestVectors.Get<double>("ts1.is_struct1.to_client");
            o.Add(748, s);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            teststruct2 s = value[372];
            ca<double>(s.mydat, TestVectors.Get<double>("ts1.is_struct1.to_service"));
        }
    }

    public Dictionary<string, teststruct2> is_struct2
    {
        get {
            Dictionary<string, teststruct2> o = new Dictionary<string, teststruct2>();
            teststruct2 s = new teststruct2();
            s.mydat = TestVectors.Get<double>("ts1.is_struct2.to_client");
            o.Add("testval1", s);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            teststruct2 s = value["testval2"];
            ca<double>(s.mydat, TestVectors.Get<double>("ts1.is_struct2.to_service"));
        }
    }

    public List<double> list_d1
    {
        get {
            List<double> o = new List<double>();
            o.Add(1.643392e-01);
            o.Add(1.537133e+09);
            o.Add(1.369505e-03);
            return o;
        }

        set {
            RRAssert.AreEqual(value.Count, 3);
            RRAssert.AreEqual(value[0], 4.074501e-07);
            RRAssert.AreEqual(value[1], -4.535303e+05);
            RRAssert.AreEqual(value[2], -2.956241e-20);
        }
    }

    public List<double[]> list_d3
    {
        get {
            List<double[]> o = new List<double[]>();
            o.Add(TestVectors.Get<double>("ts1.is_d3.to_client.a"));
            o.Add(TestVectors.Get<double>("ts1.is_d3.to_client.b"));
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 2);
            ca<double>(value[0], TestVectors.Get<double>("ts1.is_d3.to_service.a"));
            ca<double>(value[1], TestVectors.Get<double>("ts1.is_d3.to_service.b"));
        }
    }

    public List<MultiDimArray> list_d5
    {
        get {
            MultiDimArray a = new MultiDimArray(TestVectors.Get<uint>("ts1.is_d5.to_client.dims"),
                                                TestVectors.Get<double>("ts1.is_d5.to_client"));
            List<MultiDimArray> o = new List<MultiDimArray>();
            o.Add(a);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            MultiDimArray a = value[0];
            ca<uint>(a.Dims, TestVectors.Get<uint>("ts1.is_d5.to_service.dims"));
            ca<double>((double[])a.Array_, TestVectors.Get<double>("ts1.is_d5.to_service"));
        }
    }

    public List<string> list_str1
    {
        get {
            List<string> o = new List<string>();
            o.Add("Hello server");
            return o;
        }

        set {
            RRAssert.AreEqual(value.Count, 1);
            RRAssert.AreEqual(value[0], "Hello client");
        }
    }

    public List<teststruct2> list_struct1
    {
        get {
            List<teststruct2> o = new List<teststruct2>();
            teststruct2 s = new teststruct2();
            s.mydat = TestVectors.Get<double>("ts1.is_struct1.to_client");
            o.Add(s);
            return o;
        }
        set {
            RRAssert.AreEqual(value.Count, 1);
            teststruct2 s = value[0];
            ca<double>(s.mydat, TestVectors.Get<double>("ts1.is_struct1.to_service"));
        }
    }

    public com.robotraconteur.testing.TestService2.ostruct2 struct3
    {
        get {
            com.robotraconteur.testing.TestService2.ostruct2 o = new com.robotraconteur.testing.TestService2.ostruct2();
            o.a1 = TestVectors.Get<double>("ts1.struct3.to_client");
            return o;
        }
        set {
            ca<double>(value.a1, TestVectors.Get<double>("ts1.struct3.to_service"));
        }
    }

    object m_var1 = null;

    public object var1
    {
        get {
            return m_var1;
        }
        set {
            m_var1 = value;
        }
    }
    Dictionary<int, object> m_var2 = null;

    public Dictionary<int, object> var2
    {
        get {
            return m_var2;
        }
        set {
            m_var2 = value;
        }
    }

    public object var_num
    {
        get {
            return TestVectors.Get<int>("ts1.var_num.to_client");
        }
        set {
            ca<int>((int[])value, TestVectors.Get<int>("ts1.var_num.to_service"));
        }
    }

    public object var_str
    {
        get {
            return "Hello Client!";
        }
        set {
            RRAssert.AreEqual((string)value, "Hello Server!");
        }
    }

    public object var_struct
    {
        get {
            teststruct2 s = new teststruct2();
            s.mydat = TestVectors.Get<double>("ts1.var_struct.to_client");
            return s;
        }
        set {
            ca<double>(((teststruct2)value).mydat, TestVectors.Get<double>("ts1.var_struct.to_service"));
        }
    }

    public object var_vector
    {
        get {
            Dictionary<int, object> d = new Dictionary<int, object>();
            d.Add(10, "Hello Client!");
            return d;
        }
        set {
            Dictionary<int, object> d = (Dictionary<int, object>)value;
            RRAssert.AreEqual(d.Count, 1);
            RRAssert.AreEqual(((string)d[11]), "Hello Server!");
        }
    }

    public object var_dictionary
    {
        get {
            Dictionary<string, object> d = new Dictionary<string, object>();
            d.Add("test1", "Hello Client!");
            return d;
        }
        set {
            Dictionary<string, object> d = (Dictionary<string, object>)value;
            RRAssert.AreEqual(d.Count, 1);
            RRAssert.AreEqual(((string)d["test2"]), "Hello Server!");
        }
    }

    public object var_list
    {
        get {
            List<object> d = new List<object>();
            d.Add("Hello Client!");
            return d;
        }
        set {
            List<object> d = (List<object>)value;
            RRAssert.AreEqual(d.Count, 1);
            RRAssert.AreEqual(((string)d[0]), "Hello Server!");
        }
    }

    public object var_multidimarray
    {
        get {
            double[] d = TestVectors.Get<double>("ts1.var_multidimarray.to_client");
            return new MultiDimArray(TestVectors.Get<uint>("ts1.var_multidimarray.to_client.dims"), d);
        }
        set {
            MultiDimArray a = (MultiDimArray)value;
            ca<uint>(a.Dims, TestVectors.Get<uint>("ts1.var_multidimarray.to_service.dims"));
            double[] d = TestVectors.Get<double>("ts1.var_multidimarray.to_service");
            ca<double>((double[])a.Array_, d);
        }
    }

    public double errtest
    {
        get {
            throw new Exception("This is a test");
        }
        set {
            throw new Exception("This is a test");
        }
    }

    public teststruct1 nulltest
    {
        get {
            return null;
        }
        set {
            RRAssert.AreEqual((object)value, (object)null);
        }
    }

    public void func1()
    {

        var t = RobotRaconteurNode.s.CreateTimer(1000, delegate(TimerEvent evt) {
            try
            {
                ev1();
            }
            catch (Exception)
            {}
        }, false);
        t.Start();
    }

    public void func2(double d1, double d2)
    {
        var t = RobotRaconteurNode.s.CreateTimer(1000, delegate(TimerEvent evt) {
            teststruct2 s = new teststruct2();
            s.mydat = new double[] { d2 };
            ev2(d1, s);
        }, false);
        t.Start();
    }

    TcpTransport tcptransport;

    public double func3(double d1, double d2)
    {
        try
        {
            AuthenticatedUser user = ServerEndpoint.GetCurrentAuthenticatedUser();
            if (user == null)
            {
                Console.WriteLine("No user");
            }
            else
            {
                Console.WriteLine("User: " + user.Username);
                Console.WriteLine("Privileges: " + user.Privileges.Length);
                Console.WriteLine("Login Time: " + user.LoginTime.ToString());
                Console.WriteLine("Last access: " + user.LastAccessTime.ToString());
                Console.WriteLine("Now: " + RobotRaconteurNode.s.NowUTC.ToString());
            }
        }
        catch (AuthenticationException)
        {
            Console.WriteLine("No user");
        }

        if (tcptransport.IsTransportConnectionSecure(ServerEndpoint.CurrentEndpoint))
        {

            if (tcptransport.IsSecurePeerIdentityVerified(ServerEndpoint.CurrentEndpoint))
            {
                Console.WriteLine("Peer identity is verified: " +
                                  tcptransport.GetSecurePeerIdentity(ServerEndpoint.CurrentEndpoint));
            }
            else
            {
                Console.WriteLine("Peer identity is not verified");
            }
        }

        return d1 + d2;
    }

    public int meaning_of_life()
    {
        return 42;
    }

    public void func_errtest()
    {
        throw new Exception("This is a test");
    }

    public void func_errtest1()
    {
        throw new DataTypeException("This is a test");
    }

    public void func_errtest2()
    {
        throw new testexception1("This is a test");
    }

    public void func_errtest3()
    {
        throw new testexception3("This is a test");
    }

    public event Action ev1;

    public event Action<double, teststruct2> ev2;

    sub1_impl o1 = new sub1_impl();

    public sub1 get_o1()
    {
        return o1;
    }

    Dictionary<int, sub1_impl> o2 = new Dictionary<int, sub1_impl>();

    public sub1 get_o2(int ind)
    {
        lock (o2)
        {
            if (o2.ContainsKey(ind))
                return o2[ind];

            sub1_impl o = new sub1_impl();
            o.i_ind = ind;
            o2.Add(ind, o);
            return o;
        }
    }

    Dictionary<int, sub1_impl> o3 = new Dictionary<int, sub1_impl>();
    public sub1 get_o3(int ind)
    {
        lock (o3)
        {
            if (o3.ContainsKey(ind))
                return o3[ind];

            sub1_impl o = new sub1_impl();
            o.i_ind = ind;
            o3.Add(ind, o);
            return o;
        }
    }

    Dictionary<string, sub1_impl> o4 = new Dictionary<string, sub1_impl>();
    public sub1 get_o4(string ind)
    {
        lock (o4)
        {
            if (o4.ContainsKey(ind))
                return o4[ind];

            sub1_impl o = new sub1_impl();
            o.s_ind = ind;
            o4.Add(ind, o);
            return o;
        }
    }

    TestService2SubObj o5 = new TestService2SubObj();
    public com.robotraconteur.testing.TestService2.subobj get_o5()
    {
        return o5;
    }

    object o6 = null;
    public object get_o6()
    {
        return o6;
    }

    public void o6_op(int op)
    {
        try
        {

            ServerContext.CurrentServerContext.ReleaseServicePath(ServerContext.CurrentServicePath + ".o6");
        }
        catch (ServiceException)
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
            throw new InvalidOperationException();
        }
    }

    Pipe<double[]> m_p1;

    public Pipe<double[]> p1
    {
        get {
            return m_p1;
        }
        set {
            m_p1 = (Pipe<double[]>)value;
            m_p1.PipeConnectCallback = p1_connect_callback;
        }
    }

    bool packet_sent = false;
    bool ack_recv = false;
    uint packetnum = 0;

    void p1_connect_callback(Pipe<double[]>.PipeEndpoint p)
    {

        p.PacketReceivedEvent += p1_packet_received;
        p.PacketAckReceivedEvent += p1_packet_ack_received;
        p.RequestPacketAck = true;
    }

    object p1_lock = new object();
    void p1_packet_received(Pipe<double[]>.PipeEndpoint p)
    {
        RobotRaconteurNode.s.PostToThreadPool(delegate() {
            try
            {
                Thread.Sleep(500);
                lock (p1_lock)
                {
                    while (p.Available > 0)
                    {
                        double[] dat = p.ReceivePacket();
                        uint pnum = p.SendPacket(dat);
                        if (!packet_sent)
                            packetnum = pnum;
                        packet_sent = true;
                    }
                }
            }
            catch (Exception)
            {}
        });
    }

    void p1_packet_ack_received(Pipe<double[]>.PipeEndpoint p, uint packetnum)
    {
        if (packetnum == this.packetnum)
            ack_recv = true;
    }

    Pipe<teststruct2> m_p2;
    public Pipe<teststruct2> p2
    {
        get {
            return m_p2;
        }
        set {
            m_p2 = (Pipe<teststruct2>)value;
            m_p2.PipeConnectCallback = p2_connect_callback;
        }
    }
    void p2_connect_callback(Pipe<teststruct2>.PipeEndpoint p)
    {
        p.PacketReceivedEvent += p2_packet_received;
    }

    object p2_lock = new object();
    void p2_packet_received(Pipe<teststruct2>.PipeEndpoint p)
    {
        RobotRaconteurNode.s.PostToThreadPool(delegate() {
            Thread.Sleep(500);
            lock (p2_lock)
            {
                while (p.Available > 0)
                {
                    teststruct2 dat = p.ReceivePacket();
                    p.SendPacket(dat);
                }
            }
        });
    }

    public void pipe_check_error()
    {
        RRAssert.IsTrue(ack_recv);
    }

    Pipe<double> m_broadcastpipe = null;
    PipeBroadcaster<double> m_broadcastpipe_broadcaster;
    public Pipe<double> broadcastpipe
    {
        get {
            return m_broadcastpipe;
        }
        set {
            m_broadcastpipe = value;
            m_broadcastpipe_broadcaster = new PipeBroadcaster<double>(value, 3);
            RRAssert.AreEqual(m_broadcastpipe_broadcaster.MaxBacklog, 3);
            m_broadcastpipe_broadcaster.MaxBacklog = 3;

            RobotRaconteur.Timer t = RobotRaconteurNode.s.CreateTimer(100, delegate(TimerEvent ev) {
                for (double i = 0; i < 10; i++)
                {
                    m_broadcastpipe_broadcaster.AsyncSendPacket(20);
                }
            });

            t.Start();
        }
    }

    public void test_callbacks()
    {
        cb1.GetClientFunction(ServerEndpoint.CurrentEndpoint)();
        cb2.GetClientFunction(ServerEndpoint.CurrentEndpoint)(739.2, 0.392);
        double res = cb3.GetClientFunction(ServerEndpoint.CurrentEndpoint)(34, 45);
        RRAssert.AreEqual(res, (34 + 45 + 3.14));
        RRAssert.AreEqual(cb_meaning_of_life.GetClientFunction(ServerEndpoint.CurrentEndpoint)(), 42);

        RRAssert.ThrowsException<RobotRaconteurRemoteException>(
            () => cb_errtest.GetClientFunction(ServerEndpoint.CurrentEndpoint)());
    }

    Callback<Action> m_cb1;
    public Callback<Action> cb1
    {
        get {
            return m_cb1;
        }
        set {
            m_cb1 = value;
        }
    }

    Callback<Action<double, double>> m_cb2;
    public Callback<Action<double, double>> cb2
    {
        get {
            return m_cb2;
        }
        set {
            m_cb2 = value;
        }
    }

    Callback<Func<double, double, double>> m_cb3;
    public Callback<Func<double, double, double>> cb3
    {
        get {
            return m_cb3;
        }
        set {
            m_cb3 = value;
        }
    }

    Callback<Func<int>> m_cb_meaning_of_life;
    public Callback<Func<int>> cb_meaning_of_life
    {
        get {
            return m_cb_meaning_of_life;
        }
        set {
            m_cb_meaning_of_life = value;
        }
    }

    Callback<Action> m_cb_errtest;
    public Callback<Action> cb_errtest
    {
        get {
            return m_cb_errtest;
        }
        set {
            m_cb_errtest = value;
        }
    }

    Wire<double[]> m_w1;
    public Wire<double[]> w1
    {
        get {
            return m_w1;
        }
        set {
            m_w1 = value;
            m_w1.WireConnectCallback = w1_connect_callback;
        }
    }
    void w1_connect_callback(Wire<double[]> wire, Wire<double[]>.WireConnection con)
    {
        con.WireValueChanged += w1_value_changed;
    }

    void w1_value_changed(Wire<double[]>.WireConnection con, double[] val, TimeSpec time)
    {
        con.OutValue = con.InValue;
    }

    Wire<teststruct2> m_w2;
    public Wire<teststruct2> w2
    {
        get {
            return m_w2;
        }
        set {
            m_w2 = value;
            m_w2.WireConnectCallback = w2_connect_callback;
        }
    }
    void w2_connect_callback(Wire<teststruct2> wire, Wire<teststruct2>.WireConnection con)
    {
        con.WireValueChanged += w2_value_changed;
    }

    void w2_value_changed(Wire<teststruct2>.WireConnection con, teststruct2 val, TimeSpec time)
    {
        con.OutValue = con.InValue;
    }

    Wire<MultiDimArray> m_w3;
    public Wire<MultiDimArray> w3
    {
        get {
            return m_w3;
        }
        set {
            m_w3 = value;
            m_w3.WireConnectCallback = w3_connect_callback;
        }
    }
    void w3_connect_callback(Wire<MultiDimArray> wire, Wire<MultiDimArray>.WireConnection con)
    {
        con.WireValueChanged += w3_value_changed;
    }

    void w3_value_changed(Wire<MultiDimArray>.WireConnection con, MultiDimArray val, TimeSpec time)
    {
        con.OutValue = con.InValue;
    }

    Wire<double> m_broadcastwire;
    WireBroadcaster<double> m_broadcastwire_broadcaster;

    public Wire<double> broadcastwire
    {
        get {
            return m_broadcastwire;
        }
        set {
            if (m_broadcastwire != null)
                throw new Exception("");
            m_broadcastwire = value;
            m_broadcastwire_broadcaster = new WireBroadcaster<double>(value);

            RobotRaconteur.Timer t = RobotRaconteurNode.s.CreateTimer(40, delegate(TimerEvent ev) {
                try
                {
                    for (double i = 0; i < 100; i++)
                    {
                        m_broadcastwire_broadcaster.OutValue = i;
                    }
                }
                catch
                {}
            });
            t.Start();
        }
    }

    ArrayMemory<double> m_m1 = new ArrayMemory<double>(TestVectors.Get<double>("ts1.m1.initial"));
    public ArrayMemory<double> m1
    {
        get {

            return m_m1;
        }
        set {
            throw new InvalidOperationException();
        }
    }
    MultiDimArrayMemory<double> m_m2 =
        new MultiDimArrayMemory<double>(new MultiDimArray(new uint[] { 10, 10, 10, 10, 10 }, new double[100000]));
    public MultiDimArrayMemory<double> m2
    {
        get {

            return m_m2;
        }
        set {
            throw new InvalidOperationException();
        }
    }
    MultiDimArrayMemory<byte> m_m3 =
        new MultiDimArrayMemory<byte>(new MultiDimArray(new uint[] { 1024, 1024 }, new byte[1024 * 1024]));
    public MultiDimArrayMemory<byte> m3
    {
        get {

            return m_m3;
        }
        set {
            throw new InvalidOperationException();
        }
    }
}

public class sub1_impl : sub1, IRobotRaconteurMonitorObject
{
    double[] m_d1;
    public double[] d1
    {
        get {
            return m_d1;
        }
        set {
            m_d1 = value;
        }
    }

    MultiDimArray m_d2;
    public MultiDimArray d2
    {
        get {
            return m_d2;
        }
        set {
            m_d2 = value;
        }
    }

    public string m_s_ind;

    public string s_ind
    {
        get {
            return m_s_ind;
        }
        set {}
    }

    public int m_i_ind;
    public int i_ind
    {
        get {
            return m_i_ind;
        }
        set {}
    }

    sub2_impl o2_1 = new sub2_impl();

    public sub2 get_o2_1()
    {
        return o2_1;
    }

    Dictionary<int, sub2_impl> o2_2 = new Dictionary<int, sub2_impl>();

    public sub2 get_o2_2(int ind)
    {
        if (o2_2.ContainsKey(ind))
            return o2_2[ind];
        sub2_impl o = new sub2_impl();
        o.i_ind = ind;
        o2_2[ind] = o;
        return o;
    }

    Dictionary<string, sub2_impl> o2_3 = new Dictionary<string, sub2_impl>();

    public sub2 get_o2_3(string ind)
    {
        if (o2_3.ContainsKey(ind))
            return o2_3[ind];
        sub2_impl o = new sub2_impl();
        o.s_ind = ind;
        o2_3[ind] = o;
        return o;
    }

    public void RobotRaconteurMonitorEnter()
    {
        Monitor.Enter(this);
    }

    public void RobotRaconteurMonitorEnter(int timeout)
    {
        if (!Monitor.TryEnter(this, timeout))
            throw new TimeoutException();
    }

    public void RobotRaconteurMonitorExit()
    {
        Monitor.Exit(this);
    }
}

public class sub2_impl : sub2
{
    public string m_s_ind;

    public string s_ind
    {
        get {
            return m_s_ind;
        }
        set {}
    }

    public int m_i_ind;
    public int i_ind
    {
        get {
            return m_i_ind;
        }
        set {}
    }

    string m_data;
    public string data
    {
        get {
            return m_data;
        }
        set {
            m_data = value;
        }
    }

    Dictionary<string, sub3_impl> o3_1 = new Dictionary<string, sub3_impl>();

    public sub3 get_o3_1(string ind)
    {
        if (o3_1.ContainsKey(ind))
            return o3_1[ind];
        sub3_impl o = new sub3_impl();
        o.m_ind = ind;
        o3_1[ind] = o;
        return o;
    }
}

public class sub3_impl : sub3
{
    string m_data2;
    public string data2
    {
        get {
            return m_data2;
        }
        set {
            m_data2 = value;
        }
    }

    double m_data3;
    public double data3
    {
        get {
            return m_data3;
        }
        set {
            m_data3 = value;
        }
    }

    public double add(double d)
    {
        return d + 42;
    }

    public string m_ind;
    public string ind
    {
        get {
            return m_ind;
        }
        set {}
    }
}

public class TestService2SubObj : com.robotraconteur.testing.TestService2.subobj
{
    public double add_val(double v)
    {
        return v + 1;
    }
}
}

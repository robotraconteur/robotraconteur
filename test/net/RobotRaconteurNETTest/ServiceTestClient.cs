using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using com.robotraconteur.testing.TestService1;
using com.robotraconteur.testing.TestService2;
using RobotRaconteur;
using System.Threading;
using System.IO;
using System.Threading.Tasks;

// cSpell: ignore lstruct, myind, testmdarray

namespace RobotRaconteurNETTest
{
public class ServiceTestClient
{

    public void ca<T>(T[] v1, T[] v2)
        where T : IComparable, IComparable<T>
    {
        RRAssert.AreEqual(v1.Length, v2.Length);

        for (int i = 0; i < v1.Length; i++)
        {
            RRAssert.AreEqual<T>(v1[i], v2[i]);
        }
    }

    public static void RetryUntilTimeout(Action fun, int timeout)
    {
        DateTime t = DateTime.Now;
        while (true)
        {
            try
            {
                fun();
                return;
            }
            catch (Exception)
            {
                if (DateTime.Now - t > TimeSpan.FromMilliseconds(timeout))
                {
                    throw;
                }
                Thread.Sleep(100);
            }
        }
    }

    public void RunFullTest(string url, string authurl)
    {

        ConnectService(url);
        TestProperties();
        TestFunctions();
        TestEvents();
        TestObjRefs();
        TestPipes();
        TestCallbacks();
        TestWires();
        TestMemories();

        DisconnectService();

        TestAuthentication(authurl);
        TestObjectLock(authurl);
        TestMonitorLock(url);

        TestAsync(authurl);
    }

    testroot r;

    public void ConnectService(string url)
    {
        r = (testroot)RobotRaconteurNode.s.ConnectService(url, null, null, client_callback);
    }

    private void client_callback(ServiceStub client, ClientServiceListenerEventType ev, object parameter)
    {
        Console.WriteLine("Client event " + ev + " param: " + parameter?.ToString());
    }

    public void DisconnectService()
    {
        RobotRaconteurNode.s.DisconnectService(r);
    }

    public void TestProperties()
    {
        r.d1 = 3.456;
        RRAssert.AreEqual(r.d1, 12.345);
        r.d2 = TestVectors.Get<double>("ts1.d2.to_service");
        ca<double>(r.d2, TestVectors.Get<double>("ts1.d2.to_client"));
        r.d3 = new double[] { 9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01,
                              1.969554e+12, 9.451979e-05, 5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13,
                              2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14 };
        ca<double>(r.d3,
                   new double[] { 2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11,
                                  9.818531e-21, 6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00,
                                  3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21 });
        r.d4 = new double[] { -4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11,
                              3.511191e-08,  4.759250e-07,  2.455883e+09,  4.182578e+11,  4.732337e-14,
                              -2.967313e+02, -4.139188e+14, 6.287269e+03 };
        ca<double>(r.d4, new double[] { 2.864760e-08, 3.900663e+13, 9.105789e+11, 2.943743e-15, -2.823159e-16,
                                        -3.481261e+19 });

        MultiDimArray d5_1 = new MultiDimArray(new uint[] { 8, 6, 2 }, TestVectors.Get<double>("ts1.d5.to_service"));

        r.d5 = d5_1;

        MultiDimArray d5_2 = r.d5;
        ca<uint>(d5_2.Dims, new uint[] { 5, 6, 5 });
        ca<double>((double[])d5_2.Array_, TestVectors.Get<double>("ts1.d5.to_client"));

        MultiDimArray d6_1 = new MultiDimArray(
            new uint[] { 3, 3 }, new double[] { -5.528040e-08, 3.832644e-01, -9.139211e-22, -4.919312e-05, 3.809620e-11,
                                                1.751983e-09, 2.207872e-21, 1.432794e+09, -1.970313e+11 }

        );

        r.d6 = d6_1;

        MultiDimArray d6_2 = r.d6;
        ca<uint>(d6_2.Dims, new uint[] { 3, 3 });
        ca<double>((double[])d6_2.Array_,
                   new double[] { 4.427272e-10, -1.149547e-08, -1.134096e+16, -4.932974e-03, -7.702447e-01,
                                  -3.468374e-03, -5.037849e-14, -4.140513e-08, 4.553774e+03 });

        r.s1 = 3847.9283f;
        RRAssert.AreEqual(r.s1, 7.8573f);
        r.s2 = TestVectors.Get<float>("ts1.s2.to_service");
        ca<float>(r.s2, TestVectors.Get<float>("ts1.s2.to_client"));

        r.i8_1 = 45;
        RRAssert.AreEqual(r.i8_1, -66);
        r.i8_2 = TestVectors.Get<sbyte>("ts1.i8_2.to_service");
        ca<sbyte>(r.i8_2, TestVectors.Get<sbyte>("ts1.i8_2.to_client"));

        r.u8_1 = 232;
        RRAssert.AreEqual(r.u8_1, 222);
        r.u8_2 = TestVectors.Get<byte>("ts1.u8_2.to_service");
        ca<byte>(r.u8_2, TestVectors.Get<byte>("ts1.u8_2.to_client"));

        MultiDimArray u8_3_1 = new MultiDimArray(new uint[] { 30, 10 }, TestVectors.Get<byte>("ts1.u8_3.to_service"));
        r.u8_3 = u8_3_1;
        MultiDimArray u8_3_2 = r.u8_3;
        ca<uint>(new uint[] { 10, 20 }, u8_3_2.Dims);
        ca<byte>((byte[])u8_3_2.Array_, TestVectors.Get<byte>("ts1.u8_3.to_client"));

        r.i16_1 = 2387;
        RRAssert.AreEqual(r.i16_1, -13428);
        r.i16_2 = TestVectors.Get<short>("ts1.i16_2.to_service");
        ca<short>(r.i16_2, TestVectors.Get<short>("ts1.i16_2.to_client"));

        r.u16_1 = 54732;
        RRAssert.AreEqual(r.u16_1, 60981);
        r.u16_2 = TestVectors.Get<ushort>("ts1.u16_2.to_service");
        ca<ushort>(r.u16_2, TestVectors.Get<ushort>("ts1.u16_2.to_client"));

        r.i32_1 = -9837284;
        RRAssert.AreEqual(r.i32_1, 898734);
        r.i32_2 = TestVectors.Get<int>("ts1.i32_2.to_service");
        ca<int>(r.i32_2, TestVectors.Get<int>("ts1.i32_2.to_client"));

        {
            int[] i32_huge_1 = new int[2621440];
            for (int i = 0; i < 2621440; i++)
            {
                i32_huge_1[i] = -i;
            }
            r.i32_huge = i32_huge_1;
        }
        {
            var i32_huge_2 = r.i32_huge;
            RRAssert.AreEqual(i32_huge_2.Length, 2621440);
            for (int i = 0; i < 2621440; i++)
            {
                RRAssert.AreEqual(i32_huge_2[i], i);
            }
        }

        r.u32_1 = 1550099195;
        RRAssert.AreEqual<uint>(r.u32_1, 547919123);
        r.u32_2 = TestVectors.Get<uint>("ts1.u32_2.to_service");
        ca<uint>(r.u32_2, TestVectors.Get<uint>("ts1.u32_2.to_client"));

        r.i64_1 = 8621740821050813024;
        RRAssert.AreEqual(r.i64_1, -1357833931563696072);
        r.i64_2 = TestVectors.Get<long>("ts1.i64_2.to_service");
        ca<long>(r.i64_2, TestVectors.Get<long>("ts1.i64_2.to_client"));

        r.u64_1 = 1465640522145789825;
        RRAssert.AreEqual<ulong>(r.u64_1, 13389861970863644378);
        r.u64_2 = TestVectors.Get<ulong>("ts1.u64_2.to_service");
        ca<ulong>(r.u64_2, TestVectors.Get<ulong>("ts1.u64_2.to_client"));

        r.str1 = "Hello Server!";
        RRAssert.AreEqual(r.str1, "Hello Client!");

        // Read in and check the large structure struct1
        teststruct1 s1 = r.struct1;
        ca<double>(s1.dat1, TestVectors.Get<double>("ts1.struct1.dat1.to_client"));
        RRAssert.AreEqual(s1.str2, "Hello world!");
        RRAssert.AreEqual(s1.vec3.Count, 3);
        RRAssert.AreEqual(s1.vec3[1], "Hello Client!");
        RRAssert.AreEqual(s1.vec3[2], "Hello Client, again");
        RRAssert.AreEqual(s1.vec3[4372], "This is yet another test string");
        RRAssert.AreEqual(s1.dict4.Count, 3);
        RRAssert.AreEqual(s1.dict4["teststring1"], "Hello Client!");
        RRAssert.AreEqual(s1.dict4["teststring2"], "Hello Client, again");
        RRAssert.AreEqual(s1.dict4["anotherstr"], "This is yet another test string");

        RRAssert.AreEqual(s1.list5.Count, 3);
        RRAssert.AreEqual(s1.list5[0], "Hello Client!");
        RRAssert.AreEqual(s1.list5[1], "Hello Client, again");
        RRAssert.AreEqual(s1.list5[2], "This is yet another test string");

        ca<double>(s1.struct1.mydat, TestVectors.Get<double>("ts1.struct1.struct1_mydat.to_client"));

        RRAssert.AreEqual(s1.dstruct2.Count, 2);
        ca<double>(s1.dstruct2["test1"].mydat, TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_client"));
        ca<double>(s1.dstruct2["anothertest"].mydat,
                   new double[] { 4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08,
                                  -4.969544e-21, 2.530419e-01, 4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17,
                                  1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13,
                                  -1.937982e+02 });

        RRAssert.AreEqual(s1.lstruct3.Count, 2);
        ca<double>(s1.lstruct3[0].mydat, TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_client"));
        ca<double>(s1.lstruct3[1].mydat,
                   new double[] { 4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08,
                                  -4.969544e-21, 2.530419e-01, 4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17,
                                  1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13,
                                  -1.937982e+02 });

        ca<uint>(s1.multidimarray.Dims, new uint[] { 10, 10 });
        ca<double>((double[])s1.multidimarray.Array_, TestVectors.Get<double>("ts1.struct1.multidimarray.to_client"));

        RRAssert.AreEqual((string)s1.var3, "This is a vartype string");

        // Set large structure for struct1
        teststruct1 s2 = new teststruct1();
        s2.dat1 = TestVectors.Get<double>("ts1.struct1.dat1.to_service");
        s2.str2 = "Hello world 2!";
        Dictionary<int, string> s2_3 = new Dictionary<int, string>();
        s2_3.Add(10, "Hello Server!");
        s2_3.Add(11, "Hello Server, again");
        s2_3.Add(46372, "Test string!");
        s2_3.Add(46373, "Test string again");

        s2.vec3 = s2_3;

        Dictionary<string, string> s2_4 = new Dictionary<string, string>();
        s2_4.Add("cteststring1", "Hello Server!");
        s2_4.Add("cteststring2", "Hello Server, again");
        s2.dict4 = s2_4;

        List<string> s2_5 = new List<string>();
        s2_5.Add("Hello Server!");
        s2_5.Add("Hello Server, again");
        s2.list5 = s2_5;

        s2.struct1 = new teststruct2();
        s2.struct1.mydat = TestVectors.Get<double>("ts1.struct1.struct1_mydat.to_service");

        Dictionary<string, teststruct2> s2_ds2 = new Dictionary<string, teststruct2>();
        teststruct2 s2_ds2_1 = new teststruct2();
        s2_ds2_1.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_service");
        s2_ds2.Add("ctest1", s2_ds2_1);
        teststruct2 s2_ds2_2 = new teststruct2();
        s2_ds2_2.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_2.to_service");
        s2_ds2.Add("anothertest", s2_ds2_2);
        s2.dstruct2 = s2_ds2;

        List<teststruct2> s2_ls3 = new List<teststruct2>();
        teststruct2 s2_ls3_1 = new teststruct2();
        s2_ls3_1.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_1.to_service");
        s2_ls3.Add(s2_ls3_1);
        teststruct2 s2_ls3_2 = new teststruct2();
        s2_ls3_2.mydat = TestVectors.Get<double>("ts1.struct1.dstruct2_2.to_service");
        s2_ls3.Add(s2_ls3_2);
        s2.lstruct3 = s2_ls3;

        s2.multidimarray =
            new MultiDimArray(new uint[] { 10, 10 }, TestVectors.Get<double>("ts1.struct1.multidimarray.to_service"));
        s2.var3 = new double[] { 6.404176e-12,  9.258110e-03,  8.657620e-03, -2.064381e+00, 5.182360e-16,
                                 4.167658e-16,  -4.533051e-19, 5.357520e+18, -4.990383e-13, 2.286982e+08,
                                 -4.727256e-18, 1.465299e-17,  3.000340e-10, -2.304453e-04 };
        r.struct1 = s2;

        // Check struct2 in/out
        teststruct2 s3 = new teststruct2();
        s3.mydat = TestVectors.Get<double>("ts1.struct2.to_service");
        r.struct2 = s3;
        teststruct2 s4 = r.struct2;
        ca<double>(s4.mydat, TestVectors.Get<double>("ts1.struct2.to_client"));

        // Test is_d1
        Dictionary<int, double> is_d1_1 = r.is_d1;
        RRAssert.AreEqual(is_d1_1.Count, 3);
        RRAssert.AreEqual(is_d1_1[9285], 1.643392e-01);
        RRAssert.AreEqual(is_d1_1[74822], 1.537133e+09);
        RRAssert.AreEqual(is_d1_1[4], 1.369505e-03);

        Dictionary<int, double> is_d1_2 = new Dictionary<int, double>();
        is_d1_2.Add(928, 4.074501e-07);
        is_d1_2.Add(394820, -4.535303e+05);
        is_d1_2.Add(623, -2.956241e-20);
        r.is_d1 = is_d1_2;

        // Test is_d2
        Dictionary<string, double> is_d2_1 = r.is_d2;
        RRAssert.AreEqual(is_d2_1.Count, 2);
        RRAssert.AreEqual(is_d2_1["testval1"], -1.079664e+16);
        RRAssert.AreEqual(is_d2_1["testval2"], 2.224846e+00);

        Dictionary<string, double> is_d2_2 = new Dictionary<string, double>();
        is_d2_2.Add("testval3", 5.242474e+10);
        is_d2_2.Add("testval4", 2.208636e+08);
        r.is_d2 = is_d2_2;

        // Test is_d3

        Dictionary<int, double[]> is_d3_1 = r.is_d3;
        RRAssert.AreEqual(is_d3_1.Count, 2);
        ca<double>(is_d3_1[12],
                   new double[] { 8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                                  2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                                  -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                                  -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01 });
        ca<double>(is_d3_1[832],
                   new double[] { 4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                                  4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                                  2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                                  1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02 });

        Dictionary<int, double[]> is_d3_2 = new Dictionary<int, double[]>();
        is_d3_2.Add(47, new double[] { 4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                       -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                       4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                       -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15 });
        is_d3_2.Add(324, new double[] { 3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                        7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                        5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                        -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05 });
        r.is_d3 = is_d3_2;

        // Test is_d4

        Dictionary<string, double[]> is_d4_1 = r.is_d4;
        RRAssert.AreEqual(is_d4_1.Count, 2);
        ca<double>(is_d4_1["testval1"],
                   new double[] { 1.113851e-04,  3.830104e+07, 4.571169e-21,  -4.064180e-05, 2.889736e+01,
                                  -1.790060e-06, 4.608538e+00, 4.687713e-04,  1.387717e-08,  3.914187e-18,
                                  -5.618118e-06, 1.530811e+05, -5.848922e-11, -3.397558e-20, -6.597368e-08,
                                  -3.779049e-06, 2.406033e-19, 2.507939e-10,  3.246113e-20,  1.341205e+16 });
        ca<double>(is_d4_1["testval2"],
                   new double[] { -3.088190e-13, -4.033334e-20, 4.150103e-21,  -6.610855e+17, 3.688824e-13,
                                  -3.208025e+13, -5.034888e-11, -4.098363e-06, -1.272830e-03, 2.748392e-03,
                                  -2.644272e-06, -4.810065e-18, 4.629861e-19,  -5.444015e-03, 4.046008e+17,
                                  -3.548079e+12, -3.455290e+16, -3.668946e-12, -3.522178e-01, -1.537583e+14 });

        Dictionary<string, double[]> is_d4_2 = new Dictionary<string, double[]>();
        is_d4_2.Add("testval3",
                    new double[] { 1.771838e+06, 3.037284e-01,  -1.739742e-02, 1.399508e-20,  3.605232e-21,
                                   3.517522e+14, 4.887514e+14,  3.505442e-03,  -3.968972e+18, 1.422037e-20,
                                   2.596937e-21, 4.852833e-11,  6.852955e-17,  4.765526e-12,  -3.445954e+16,
                                   2.322531e-14, -1.755122e-12, 3.941875e+00,  8.877046e-13,  2.818923e-02 });
        is_d4_2.Add("testval4",
                    new double[] { 4.146439e+16,  2.923439e-07,  3.549608e+16,  -1.664891e-01, -4.192309e-15,
                                   3.857317e+05,  -1.101076e+00, 1.213105e+19,  3.237584e-14,  -2.421219e-06,
                                   -4.603196e-05, -3.719535e-10, 1.124961e+06,  2.032849e+10,  4.639704e-22,
                                   3.946835e+01,  -9.267263e+01, -4.456188e+11, 3.470487e+08,  7.918764e+10 });
        r.is_d4 = is_d4_2;

        // Test is_d5
        Dictionary<int, MultiDimArray> is_d5_1 = r.is_d5;
        RRAssert.AreEqual(is_d5_1.Count, 1);
        MultiDimArray is_d5_1_1 = is_d5_1[564];
        ca<uint>(is_d5_1_1.Dims, new uint[] { 10, 10 });
        ca<double>((double[])is_d5_1_1.Array_, TestVectors.Get<double>("ts1.is_d5.to_client"));

        MultiDimArray is_d5_2_1 =
            new MultiDimArray(new uint[] { 10, 20 }, TestVectors.Get<double>("ts1.is_d5.to_service"));
        Dictionary<int, MultiDimArray> is_d5_2 = new Dictionary<int, MultiDimArray>();
        is_d5_2.Add(328, is_d5_2_1);

        // Test is_d6
        Dictionary<string, MultiDimArray> is_d6_1 = r.is_d6;
        RRAssert.AreEqual(is_d6_1.Count, 1);
        MultiDimArray is_d6_1_1 = is_d6_1["testval1"];
        ca<uint>(is_d6_1_1.Dims, new uint[] { 5, 10 });
        ca<double>((double[])is_d6_1_1.Array_, TestVectors.Get<double>("ts1.is_d6.to_client"));

        MultiDimArray is_d6_2_1 =
            new MultiDimArray(new uint[] { 8, 10 }, TestVectors.Get<double>("ts1.is_d6.to_service"));
        Dictionary<string, MultiDimArray> is_d6_2 = new Dictionary<string, MultiDimArray>();
        is_d6_2.Add("testval2", is_d6_2_1);
        r.is_d6 = is_d6_2;

        // Test is_str1
        Dictionary<int, string> is_str1_1 = r.is_str1;
        RRAssert.AreEqual(is_str1_1.Count, 1);
        RRAssert.AreEqual(is_str1_1[23], "Hello server");

        Dictionary<int, string> is_str1_2 = new Dictionary<int, string>();
        is_str1_2.Add(24, "Hello client");
        r.is_str1 = is_str1_2;

        // Test is_str2
        Dictionary<string, string> is_str2_1 = r.is_str2;
        RRAssert.AreEqual(is_str2_1.Count, 1);
        RRAssert.AreEqual(is_str2_1["testval1"], "Hello server");

        Dictionary<string, string> is_str2_2 = new Dictionary<string, string>();
        is_str2_2.Add("testval2", "Hello client");
        r.is_str2 = is_str2_2;

        // Test is_struct1
        Dictionary<int, teststruct2> is_struct1_1 = r.is_struct1;
        ca<double>(is_struct1_1[748].mydat,
                   new double[] { -9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                                  -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                                  -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                                  -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21 });

        Dictionary<int, teststruct2> is_struct1_2 = new Dictionary<int, teststruct2>();
        teststruct2 is_struct1_2_1 = new teststruct2();
        is_struct1_2_1.mydat =
            new double[] { -2.101948e-07, -2.594836e-08, 2.515710e+01,  -3.834127e-14, -3.088095e+06,
                           -3.256612e-02, -1.855481e-19, 3.801916e+07,  2.145894e+09,  4.487676e+12,
                           1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00, 1.786029e-20,
                           -4.991515e+08, 4.006107e-10,  -4.947871e-11, -2.737020e-08, 4.123759e-20 };
        is_struct1_2.Add(372, is_struct1_2_1);
        r.is_struct1 = is_struct1_2;

        // Test is_struct2
        Dictionary<string, teststruct2> is_struct2_1 = r.is_struct2;
        ca<double>(is_struct2_1["testval1"].mydat,
                   new double[] { -4.489570e+13, 9.574895e-05,  4.081711e+06,  5.612839e-18,  -1.078604e+05,
                                  3.658139e+08,  -4.748975e+05, -2.606481e+01, 3.016739e+15,  3.174709e+19,
                                  -4.572549e+17, 1.980389e-04,  -3.551911e-10, 3.598401e-07,  2.659416e-12,
                                  -3.606157e+06, 2.059674e+17,  -9.362336e-20, -3.299256e+17, -2.245745e+16 });

        Dictionary<string, teststruct2> is_struct2_2 = new Dictionary<string, teststruct2>();
        teststruct2 is_struct2_2_1 = new teststruct2();
        is_struct2_2_1.mydat =
            new double[] { 6.931327e-21,  4.527137e-02,  1.260822e-18,  3.592805e-12,  1.088317e-05,
                           3.305865e+03,  -9.798828e-20, 1.497504e+18,  -3.653592e+01, 1.473952e+10,
                           -1.003612e-20, 1.302159e+18,  -8.544326e+05, 1.038521e+16,  -2.845746e-18,
                           -3.899909e-04, 4.785560e-02,  -7.203365e-12, -1.500022e-14, -1.892753e-17 };
        is_struct2_2.Add("testval2", is_struct2_2_1);
        r.is_struct2 = is_struct2_2;

        // Test list_d1
        List<double> list_d1_1 = r.list_d1;
        RRAssert.AreEqual(list_d1_1.Count, 3);
        RRAssert.AreEqual(list_d1_1[0], 1.643392e-01);
        RRAssert.AreEqual(list_d1_1[1], 1.537133e+09);
        RRAssert.AreEqual(list_d1_1[2], 1.369505e-03);

        List<double> list_d1_2 = new List<double>();
        list_d1_2.Add(4.074501e-07);
        list_d1_2.Add(-4.535303e+05);
        list_d1_2.Add(-2.956241e-20);
        r.list_d1 = list_d1_2;

        // Test list_d3
        List<double[]> list_d3_1 = r.list_d3;
        RRAssert.AreEqual(list_d3_1.Count, 2);
        ca<double>(list_d3_1[0],
                   new double[] { 8.609080e-13,  3.946603e+03,  2.994203e-10,  3.200877e+14,  1.747361e-09,
                                  2.827056e-16,  -3.676613e-18, 1.886901e-14,  -9.970511e-12, 1.932468e-18,
                                  -3.629253e-05, 4.903023e-12,  -3.919949e-10, 4.982164e+07,  3.823096e-20,
                                  -4.044068e-13, 3.114078e+09,  7.572697e-12,  -2.619929e+04, -3.882046e+01 });
        ca<double>(list_d3_1[1],
                   new double[] { 4.750899e+00, 3.924377e+18,  -2.735066e+17, 4.095362e-21,  -2.407932e+09,
                                  4.059499e+10, 1.376975e-10,  -8.547220e-21, -1.344568e-20, 2.809398e+03,
                                  2.118944e-06, 2.435328e-03,  -1.410999e-12, 9.907226e-04,  -9.745948e-20,
                                  1.270118e+15, -2.833333e+05, 1.032636e-10,  5.312574e+13,  -2.651512e+02 });

        List<double[]> list_d3_2 = new List<double[]>();
        list_d3_2.Add(new double[] { 4.335907e-08,  -3.270294e-03, 1.752801e-01,  1.235219e-20,  -4.348647e+02,
                                     -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13,  -3.380846e-05,
                                     4.350998e+03,  4.539570e+11,  8.981827e+09,  3.326114e+01,  2.975688e+06,
                                     -1.017456e-12, 2.989498e-03,  2.842392e-03,  -1.258677e-21, 1.068563e-15 });
        list_d3_2.Add(new double[] { 3.239279e+12,  1.047689e+17,  -1.236114e+17, -4.002822e-17, 2.657374e-03,
                                     7.383907e-19,  -5.067889e-13, -4.195122e-12, 3.642885e-01,  -2.946040e+14,
                                     5.522403e-08,  6.603132e+04,  1.464154e+05,  -1.851534e-08, 2.808294e-13,
                                     -2.702278e-11, 3.850704e-06,  -2.453957e+02, -3.015401e-02, 1.654070e+05 });
        r.list_d3 = list_d3_2;

        // Test list_d5
        List<MultiDimArray> list_d5_1 = r.list_d5;
        RRAssert.AreEqual(list_d5_1.Count, 1);
        MultiDimArray list_d5_1_1 = list_d5_1[0];
        ca<uint>(list_d5_1_1.Dims, new uint[] { 10, 10 });
        ca<double>((double[])list_d5_1_1.Array_, TestVectors.Get<double>("ts1.is_d5.to_client"));

        MultiDimArray list_d5_2_1 =
            new MultiDimArray(new uint[] { 10, 20 }, TestVectors.Get<double>("ts1.is_d5.to_service"));
        List<MultiDimArray> list_d5_2 = new List<MultiDimArray>();
        list_d5_2.Add(is_d5_2_1);

        // Test list_str1
        List<string> list_str1_1 = r.list_str1;
        RRAssert.AreEqual(list_str1_1.Count, 1);
        RRAssert.AreEqual(list_str1_1[0], "Hello server");

        List<string> list_str1_2 = new List<string>();
        list_str1_2.Add("Hello client");
        r.list_str1 = list_str1_2;

        // Test list_struct1
        List<teststruct2> list_struct1_1 = r.list_struct1;
        ca<double>(list_struct1_1[0].mydat,
                   new double[] { -9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20,  5.147581e-14,
                                  -3.773975e+15, 2.336430e-21,  1.597144e-18,  -2.609059e-03, 3.557639e-21,
                                  -1.666575e-16, -4.242788e-07, 2.686206e+07,  -3.200902e-05, -1.549754e-06,
                                  -3.010796e-12, 4.638418e+01,  2.664397e-14,  -2.689174e+01, 4.564584e-21 });

        List<teststruct2> list_struct1_2 = new List<teststruct2>();
        teststruct2 list_struct1_2_1 = new teststruct2();
        list_struct1_2_1.mydat =
            new double[] { -2.101948e-07, -2.594836e-08, 2.515710e+01,  -3.834127e-14, -3.088095e+06,
                           -3.256612e-02, -1.855481e-19, 3.801916e+07,  2.145894e+09,  4.487676e+12,
                           1.351202e-02,  -1.125124e-16, 1.369826e-20,  -2.290673e+00, 1.786029e-20,
                           -4.991515e+08, 4.006107e-10,  -4.947871e-11, -2.737020e-08, 4.123759e-20 };
        list_struct1_2.Add(is_struct1_2_1);
        r.list_struct1 = list_struct1_2;

        // Test struct3
        ca<double>(r.struct3.a1,
                   new double[] { -8.483090e-19, -4.401548e-08, 3.908118e+00,  2.063513e-18,  4.237047e+18,
                                  -1.124681e-16, 3.924541e-01,  -2.184335e-10, -1.978950e+11, 1.586365e+18,
                                  1.712393e+00,  -6.314723e+00, 1.196777e-16,  -2.748704e-08, -1.289967e+02,
                                  -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06,  1.315398e+17 });

        ostruct2 struct3_1 = new ostruct2();
        struct3_1.a1 = new double[] { -2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03,
                                      8.256717e-12,  -4.772119e-11, -1.061407e-13, 2.759750e+02,  -1.212549e+10,
                                      7.012690e+15,  3.953354e+04,  -2.617985e-07, 1.104408e-21,  -3.889366e+00,
                                      4.549493e+16,  -1.376791e+15, -3.445205e-21, 2.137830e-14,  4.620179e+18 };
        r.struct3 = struct3_1;

        // Test var1  TODO...

        // Test var2 TODO...

        // Test var_num
        ca<int>((int[])r.var_num, new int[] { -1680284833, -54562307, 732107275, 1470526962, -1389452949, 256801409,
                                              261288152, 1728150828, 1322531658, -1640628174, 1036878614, 511108054,
                                              2057847386, 288780916, 996595759 });
        r.var_num = new int[] { -1046369769, 1950632347, 1140727074, -1277424443, 163999900,
                                970815027,   545593183,  514305170,  1896372264,  1385916382 };

        // Test var_str
        RRAssert.AreEqual((string)r.var_str, "Hello Client!");
        r.var_str = "Hello Server!";

        // Test var_struct
        ca<double>(((teststruct2)r.var_struct).mydat,
                   new double[] { -9.052731e+13, 4.151705e-17,  -4.004463e+19, -2.838274e+03, 9.983314e+12,
                                  2.764122e+10,  -1.131486e+03, 2.418899e+12,  1.323675e-05,  -4.602174e+13,
                                  2.717530e+01,  1.193887e-10,  -4.137578e+16, -1.246990e-19, 4.244315e-18,
                                  -2.833005e-08, 1.956266e-04,  4.130129e-21,  1.641708e-11,  -4.488158e-19 });

        teststruct2 var_struct_1 = new teststruct2();
        var_struct_1.mydat = new double[] { -4.945426e-20, 1.763386e+13,  3.431578e-04,  4.411409e+17,  -2.690201e+03,
                                            3.025939e-10,  -3.659846e+11, -4.780435e-10, -3.246816e+14, -1.815578e+04,
                                            2.236455e+10,  -4.639041e+14, 1.767930e+10,  -1.636094e+05, -4.392462e-01,
                                            2.225260e+04,  -5.250245e+18, 8.755282e-12,  2.005819e-10,  2.702210e+04 };
        r.var_struct = var_struct_1;

        // Test var_vector
        RRAssert.AreEqual(((string)((Dictionary<int, object>)r.var_vector)[10]), "Hello Client!");
        Dictionary<int, object> var_vector_1 = new Dictionary<int, object>();
        var_vector_1.Add(11, "Hello Server!");
        r.var_vector = var_vector_1;

        // Test var_dictionary
        RRAssert.AreEqual(((string)((Dictionary<string, object>)r.var_dictionary)["test1"]), "Hello Client!");
        Dictionary<string, object> var_dictionary_1 = new Dictionary<string, object>();
        var_dictionary_1.Add("test2", "Hello Server!");
        r.var_dictionary = var_dictionary_1;

        // Test var_list
        RRAssert.AreEqual(((string)((List<object>)r.var_list)[0]), "Hello Client!");
        List<object> var_list_1 = new List<object>();
        var_list_1.Add("Hello Server!");
        r.var_list = var_list_1;

        // Test var_multidimarray
        MultiDimArray var_a_1 = (MultiDimArray)r.var_multidimarray;
        ca<uint>(var_a_1.Dims, new uint[] { 5, 4 });
        ca<double>((double[])var_a_1.Array_,
                   new double[] { -4.915597e-01, 3.892823e+00,  2.622325e+08,  -7.150935e+04, 9.418756e+00,
                                  3.633879e+18,  3.522383e-03,  -4.989811e+05, 2.027383e-03,  -3.153241e+12,
                                  -6.948245e-21, -3.198577e+14, 6.172905e+09,  3.849430e+15,  8.600383e+13,
                                  4.079437e-17,  3.194775e+06,  4.222550e-18,  1.758122e+17,  -1.018308e+03 });
        r.var_multidimarray = new MultiDimArray(new uint[] { 5, 4 }, new double[] {
            3.792953e+00, 2.968121e-17, -3.976413e-15, 4.392986e+19,  2.197463e+10,  -2.627743e-14, -2.184665e+17,
            1.972257e-17, 9.929684e-03, -3.096821e+17, 3.598051e+11,  -6.266015e-18, 1.811985e-11,  2.815232e-07,
            7.469467e-06, 6.141798e+13, 3.105763e+09,  -1.697809e-10, -4.141707e-17, 4.391634e+13
        });

        // Test errtest and make sure an exception is thrown

        RRAssert.ThrowsException<Exception>(delegate() { r.errtest = 1; });

        RRAssert.ThrowsException<Exception>(delegate() { double d = r.errtest; });

        // Test nulltest
        RRAssert.AreEqual((object)r.nulltest, (object)null);
        r.nulltest = null;
    }

    public void TestFunctions()
    {

        r.func1();
        r.func2(10, 20.34);
        RRAssert.AreEqual(r.func3(2, 3.45), 5.45);
        RRAssert.AreEqual(r.meaning_of_life(), 42);

        RRAssert.ThrowsException<Exception>(delegate() { r.func_errtest(); });

        RRAssert.ThrowsException<DataTypeException>(delegate() { r.func_errtest1(); });

        RRAssert.ThrowsException<testexception1>(delegate() { r.func_errtest2(); });

        RRAssert.ThrowsException<testexception3>(delegate() { r.func_errtest3(); });
    }

    bool ev1_set = false;
    bool ev2_set = false;

    public void TestEvents()
    {
        Console.WriteLine("Begin event");
        r.ev1 += ev1_cb;
        r.ev2 += ev2_cb;

        ev1_set = false;
        ev2_set = false;
        try
        {
            ev1_event = new System.Threading.AutoResetEvent(false);
            ev2_event = new System.Threading.AutoResetEvent(false);

            r.func1();

            r.func2(27.3, 98.23);

            if (!ev1_set)
                RRAssert.IsTrue(ev1_event.WaitOne(5000));

            if (!ev2_set)
                RRAssert.IsTrue(ev2_event.WaitOne(5000));
        }
        finally
        {
            r.ev1 -= ev1_cb;
            r.ev2 -= ev2_cb;

            ev1_event = null;
            ev2_event = null;
        }
    }

    System.Threading.AutoResetEvent ev1_event;
    private void ev1_cb()
    {
        ev1_set = true;
        ev1_event.Set();
        Console.WriteLine("Got ev1");
    }

    System.Threading.AutoResetEvent ev2_event;
    private void ev2_cb(double d, teststruct2 s)
    {
        ev2_set = true;
        ev2_event.Set();
        Console.WriteLine("Got ev2 {0}, {1}", d, s.mydat[0]);
        if (d == 27.3 && s.mydat[0] == 98.23)
        {}
    }

    public void TestObjRefs()
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

        o1.d1 = new double[] { -2.086627e+06, 3.092642e+04,  -1.981667e+02, 1.963286e-20, 4.264052e-08,
                               3.594320e+12,  -4.820517e-02, -3.629590e+06, 6.037089e-07, 3.328419e+06 };
        o2_10.d1 = new double[] { 4.978178e-14,  2.867603e-17,  4.471047e-21,  -2.002902e+15, -2.910881e-03,
                                  -2.601092e-03, -3.043199e+16, -3.257109e-12, 1.834255e-11,  -3.383015e+00 };
        o2_34.d1 = new double[] { 4.661927e-02,  2.334444e+02, 3.985567e+12, -2.324843e+01, -3.315866e+03,
                                  -4.442404e+10, 3.280626e-02, 2.334668e-12, -3.374202e-14, 4.809260e+02 };
        o3_1.d1 = new double[] { -1.882441e-04, 2.065458e+14, -6.309214e-16, -3.181637e-07, -9.906616e+02,
                                 1.684926e-14,  1.672252e+15, -3.950901e+01, -3.295950e-17, -3.080902e+13 };
        o4_myind.d1 = new double[] { 2.997950e-14,  -1.077977e+17, 3.721399e-09, -1.289619e+18, 4.494844e+06,
                                     -4.918719e-15, 2.194759e+13,  2.554572e-09, 4.166299e-06,  -1.409589e+04 };
        o4_specialind.d1 = new double[] { 3.404179e-08,  -1.749189e-18, -3.219593e-09, 1.313794e+01,  -4.193673e+10,
                                          -2.479829e+07, -2.617068e+04, 8.181730e+15,  -2.003653e+18, -1.833401e+19 };
        o2_10_o2_1_32.data = "Hello world!";
        o2_10_o2_1_32_o3_ind1.data2 = "Test string 1";
        o2_10_o2_1_32_o3_ind2.data2 = "Test string 2";

        // Test the varobject type and make sure the ReleaseServicePath works...

        sub1 o6_1 = null;
        sub2 o6_1_1 = null;
        r.o6_op(0);
        RetryUntilTimeout(delegate() {
            o6_1 = (sub1)r.get_o6();
            o6_1_1 = o6_1.get_o2_1();
            o6_1.d1 = new double[] { 0.0 };
            o6_1_1.data = "Hello world!";
        }, 1000);

        r.o6_op(1);

        sub2 o6_2 = null;
        RetryUntilTimeout(delegate() {
            RRAssert.ThrowsException<Exception>(delegate() { o6_1.d1 = new double[] { 0.0 }; });
            RRAssert.ThrowsException<Exception>(delegate() { o6_1_1.data = "Hello world!"; });

            o6_2 = (sub2)r.get_o6();
            o6_2.data = "Hello world!";
        }, 1000);

        r.o6_op(2);

        RetryUntilTimeout(delegate() {
            RRAssert.ThrowsException<Exception>(delegate() { o6_2.data = "Hello world!"; });

            com.robotraconteur.testing.TestService2.subobj o6_3 =
                (com.robotraconteur.testing.TestService2.subobj)r.get_o6();
            o6_3.add_val(2);
        }, 1000);
    }

    System.Threading.AutoResetEvent ee1;
    System.Threading.AutoResetEvent ee2;
    System.Threading.AutoResetEvent ee3;

    uint packetnum = 0;
    bool ack_recv = false;

    public void TestPipes()
    {

        ee1 = new System.Threading.AutoResetEvent(false);
        ee2 = new System.Threading.AutoResetEvent(false);
        ee3 = new System.Threading.AutoResetEvent(false);

        Pipe<double[]>.PipeEndpoint e1 = r.p1.Connect(-1);
        Pipe<double[]>.PipeEndpoint e2 = r.p1.Connect(3432);
        Pipe<teststruct2>.PipeEndpoint e3 = r.p2.Connect(-1);

        e1.RequestPacketAck = true;

        e1.PacketReceivedEvent += ee1_cb;
        e1.PacketAckReceivedEvent += ee1_ack_cb;
        e2.PacketReceivedEvent += ee2_cb;
        e3.PacketReceivedEvent += ee3_cb;
        lock (this)
        {
            packetnum = e1.SendPacket(new double[] { 1, 2, 3, 4 });
        }
        Console.WriteLine("Sent packet " + packetnum.ToString());
        e1.SendPacket(new double[] { 5, 6, 7, 8 });
        e1.SendPacket(new double[] { -1, -2, -3, -5.32 });

        e2.SendPacket(new double[] { 3.21 });
        e2.SendPacket(new double[] { 4.72 });
        e2.SendPacket(new double[] { 72.34 });

        teststruct2 s1 = new teststruct2();
        s1.mydat = new double[] { 738.29 };
        teststruct2 s2 = new teststruct2();
        s2.mydat = new double[] { 89.83 };

        e3.SendPacket(s1);
        e3.SendPacket(s2);

        RRAssert.IsTrue(ee1.WaitOne(5000));
        RRAssert.IsTrue(ee2.WaitOne(5000));
        RRAssert.IsTrue(ee3.WaitOne(5000));

        ca<double>(e1.ReceivePacket(), new double[] { 1, 2, 3, 4 });
        ca<double>(e1.ReceivePacketWait(1000), new double[] { 5, 6, 7, 8 });
        ca<double>(e1.ReceivePacketWait(1000), new double[] { -1, -2, -3, -5.32 });

        ca<double>(e2.ReceivePacket(), new double[] { 3.21 });
        ca<double>(e2.ReceivePacketWait(1000), new double[] { 4.72 });
        ca<double>(e2.ReceivePacketWait(1000), new double[] { 72.34 });

        ca<double>(e3.ReceivePacket().mydat, new double[] { 738.29 });
        ca<double>(e3.ReceivePacketWait(1000).mydat, new double[] { 89.83 });

        RetryUntilTimeout(() => RRAssert.IsTrue(ack_recv), 1000);
        Console.WriteLine("Packet ack received");

        RetryUntilTimeout(() => r.pipe_check_error(), 1000);
        Console.WriteLine("Pipe error checked");

        e1.Close();
        e2.Close();
        e3.Close();
    }

    void ee1_cb(Pipe<double[]>.PipeEndpoint p)
    {
        try
        {
            ee1.Set();
        }
        catch
        {}
    }

    void ee1_ack_cb(Pipe<double[]>.PipeEndpoint p, uint packetnum)
    {
        Console.WriteLine("Packet ack received: " + packetnum.ToString());
        lock (this)
        {
            if (packetnum == this.packetnum)
                ack_recv = true;
        }
    }

    void ee2_cb(Pipe<double[]>.PipeEndpoint p)
    {
        try
        {
            ee2.Set();
        }
        catch
        {}
    }

    void ee3_cb(Pipe<teststruct2>.PipeEndpoint p)
    {
        try
        {
            ee3.Set();
        }
        catch
        {}
    }

    bool cb1_called;
    bool cb2_called;
    bool cb3_called;
    bool cb4_called;
    bool cb5_called;

    public void TestCallbacks()
    {
        cb1_called = false;
        cb2_called = false;
        cb3_called = false;
        cb4_called = false;
        cb5_called = false;

        r.cb1.Function = cb1_func;
        r.cb2.Function = cb2_func;
        r.cb3.Function = cb3_func;
        r.cb_meaning_of_life.Function = cb_meaning_of_life_func;
        r.cb_errtest.Function = cb_errtest;

        r.test_callbacks();

        RRAssert.IsTrue(cb1_called);
        RRAssert.IsTrue(cb2_called);
        RRAssert.IsTrue(cb3_called);
        RRAssert.IsTrue(cb4_called);
        RRAssert.IsTrue(cb5_called);
    }

    public void cb1_func()
    {
        cb1_called = true;
    }

    public void cb2_func(double d1, double d2)
    {
        RRAssert.AreEqual(d1, 739.2);
        RRAssert.AreEqual(d2, 0.392);

        cb2_called = true;
    }

    public double cb3_func(double d1, double d2)
    {
        cb3_called = true;
        return d1 + d2 + 3.14;
    }

    public int cb_meaning_of_life_func()
    {
        cb4_called = true;
        return 42;
    }

    public void cb_errtest()
    {
        cb5_called = true;
        throw new Exception("This is a test");
    }

    bool w1_called;
    bool w2_called;
    bool w3_called;

    public void TestWires()
    {
        w1_called = false;
        w2_called = false;
        w3_called = false;

        Wire<double[]>.WireConnection w1 = r.w1.Connect();
        Wire<teststruct2>.WireConnection w2 = r.w2.Connect();
        Wire<MultiDimArray>.WireConnection w3 = r.w3.Connect();

        w1.InValueLifespan = 10000;

        w1.WireValueChanged += w1_changed;
        w2.WireValueChanged += w2_changed;
        w3.WireValueChanged += w3_changed;

        RetryUntilTimeout(delegate() {
            w1.OutValue = new double[] { 0 };

            teststruct2 s1 = new teststruct2();
            s1.mydat = new double[] { 0 };
            w2.OutValue = s1;

            MultiDimArray a1 = new MultiDimArray(new uint[] { 1, 1 }, new int[] { 0 });
            w3.OutValue = a1;

            w1.OutValue = new double[] { -2.377683e+02, -6.760080e-08, 4.191315e-18,  -4.621977e+07, -1.570323e+03,
                                         -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11 };
            teststruct2 s2 = new teststruct2();
            s2.mydat = new double[] { -1.014645e-21, 4.743740e+11,  5.804886e-04,  2.963852e-20, 4.277621e-21,
                                      -1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10 };
            w2.OutValue = s2;

            MultiDimArray a2 = new MultiDimArray(
                new uint[] { 2, 5 }, new int[] { 2058500854, -611248192, 197490486, -517717939, -513450368, 296469979,
                                                 645365194, 2043654604, -1672941174, 710030901 });
            w3.OutValue = a2;

            Thread.Sleep(500);

            double[] in1 = w1.InValue;
            ca<double>(in1, new double[] { -2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -1.570323e+03,
                                           -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11 });

            teststruct2 in2 = w2.InValue;
            ca<double>(in2.mydat,
                       new double[] { -1.014645e-21, 4.743740e+11, 5.804886e-04, 2.963852e-20, 4.277621e-21,
                                      -1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10 });

            MultiDimArray in3 = w3.InValue;
            ca<uint>(in3.Dims, new uint[] { 2, 5 });
            ca<int>((int[])in3.Array_, new int[] { 2058500854, -611248192, 197490486, -517717939, -513450368, 296469979,
                                                   645365194, 2043654604, -1672941174, 710030901 });

            w1.InValueLifespan = 1;
            Thread.Sleep(10);
            RRAssert.ThrowsException<Exception>(delegate() { double[] in1_2 = w1.InValue; });

            RRAssert.IsTrue(w1_called);
            RRAssert.IsTrue(w2_called);
            RRAssert.IsTrue(w3_called);
        }, 5000);
    }

    void w1_changed(Wire<double[]>.WireConnection c, double[] value, TimeSpec t)
    {
        w1_called = true;
    }

    void w2_changed(Wire<teststruct2>.WireConnection c, teststruct2 value, TimeSpec t)
    {
        w2_called = true;
    }

    void w3_changed(Wire<MultiDimArray>.WireConnection c, MultiDimArray value, TimeSpec t)
    {
        w3_called = true;
    }

    public void TestMemories()
    {

        test_m1();

        // Run the test twice, once with a large maximum transfer size, and one with a laughably small one
        // test_m2();
        // test_m3();
        uint l = RobotRaconteurNode.s.MemoryMaxTransferSize;
        RobotRaconteurNode.s.MemoryMaxTransferSize = 10240;
        test_m1();
        // test_m2();
        RobotRaconteurNode.s.MemoryMaxTransferSize = l;
    }

    private void test_m1()
    {
        RRAssert.AreEqual<ulong>(r.m1.Length, 100);
        double[] m1_1 = new double[11];
        r.m1.Read(10, m1_1, 1, 10);
        ca<double>(m1_1, new double[] { 0, -1.478723e-16, 1.507042e-05, -2.046271e+13, 4.014775e+06, 4.140740e+10,
                                        1.318907e+16, -2.312403e+17, 4.463696e-13, 9.173421e-04, 6.169183e-21 });

        double[] m1_2 =
            new double[] { 3.462892e+10, -1.149841e-18, 4.649317e-15,  -3.632280e-19, -5.252280e+07, 1.800453e-07,
                           3.772468e-04, -1.911891e+09, -3.018967e-14, 4.835062e-06,  5.269663e+13,  4.946221e+03 };
        r.m1.Write(20, m1_2, 2, 8);

        double[] m1_3 = new double[100];
        r.m1.Read(0, m1_3, 0, 100);

        double[] m1_4 = TestVectors.Get<double>("ts1.m1.initial");
        Array.Copy(m1_2, 2, m1_4, 20, 8);
        ca<double>(m1_3, m1_4);
    }

    private void test_m2()
    {
        RRAssert.AreEqual(r.m2.DimCount, 5);
        ulong[] m2_dims = r.m2.Dimensions;
        ca<ulong>(m2_dims, new ulong[] { 10, 10, 10, 10, 10 });

        MultiDimArray m1 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray1.bin"));
        MultiDimArray m2 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray2.bin"));
        MultiDimArray m3 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray3.bin"));
        MultiDimArray m4 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray4.bin"));
        MultiDimArray m5 =
            MultiDimArrayTest.LoadDoubleArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray5.bin"));

        r.m2.Write(new ulong[] { 0, 0, 0, 0, 0 }, m1, new ulong[] { 0, 0, 0, 0, 0 },
                   m1.Dims.Select(x => (ulong)x).ToArray());

        MultiDimArray m1_2 = new MultiDimArray(new uint[] { 10, 10, 10, 10, 10 }, new double[100000]);
        r.m2.Read(new ulong[] { 0, 0, 0, 0, 0 }, m1_2, new ulong[] { 0, 0, 0, 0, 0 },
                  m1.Dims.Select(x => (ulong)x).ToArray());
        ca<uint>(m1.Dims, m1_2.Dims);
        ca<double>((double[])m1.Array_, (double[])m1_2.Array_);

        r.m2.Write(new ulong[] { 2, 2, 3, 3, 4 }, m2, new ulong[] { 0, 2, 0, 0, 0 }, new ulong[] { 1, 5, 5, 2, 1 });
        MultiDimArray m2_2 = new MultiDimArray(new uint[] { 10, 10, 10, 10, 10 }, new double[100000]);
        r.m2.Read(new ulong[] { 0, 0, 0, 0, 0 }, m2_2, new ulong[] { 0, 0, 0, 0, 0 },
                  m1.Dims.Select(x => (ulong)x).ToArray());

        ca<uint>(m2_2.Dims, m3.Dims);
        ca<double>((double[])m2_2.Array_, (double[])m3.Array_);

        MultiDimArray m6 = new MultiDimArray(new uint[] { 2, 2, 1, 1, 10 }, new double[40]);
        r.m2.Read(new ulong[] { 4, 2, 2, 8, 0 }, m6, new ulong[] { 0, 0, 0, 0, 0 }, new ulong[] { 2, 2, 1, 1, 10 });
        ca<uint>(m4.Dims, m6.Dims);
        ca<double>((double[])m4.Array_, (double[])m6.Array_);

        MultiDimArray m7 = new MultiDimArray(new uint[] { 4, 4, 4, 4, 10 }, new double[2560]);
        r.m2.Read(new ulong[] { 4, 2, 2, 8, 0 }, m7, new ulong[] { 2, 1, 2, 1, 0 }, new ulong[] { 2, 2, 1, 1, 10 });
        ca<uint>(m5.Dims, m7.Dims);
        ca<double>((double[])m5.Array_, (double[])m7.Array_);
    }

    private void test_m3()
    {
        MultiDimArray m1 =
            MultiDimArrayTest.LoadByteArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray_b1.bin"));
        MultiDimArray m2 =
            MultiDimArrayTest.LoadByteArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray_b2.bin"));
        MultiDimArray m3 =
            MultiDimArrayTest.LoadByteArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray_b3.bin"));
        MultiDimArray m4 =
            MultiDimArrayTest.LoadByteArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray_b4.bin"));
        MultiDimArray m5 =
            MultiDimArrayTest.LoadByteArrayFromFile(Path.Combine(MultiDimArrayTest.testdatapath, "testmdarray_b5.bin"));

        r.m3.Write(new ulong[] { 0, 0 }, m1, new ulong[] { 0, 0 }, new ulong[] { 1024, 1024 });

        MultiDimArray m1_2 = new MultiDimArray(new uint[] { 1024, 1024 }, new byte[1024 * 1024]);
        r.m3.Read(new ulong[] { 0, 0 }, m1_2, new ulong[] { 0, 0 }, new ulong[] { 1024, 1024 });
        ca<uint>(m1.Dims, m1_2.Dims);
        ca<byte>((byte[])m1.Array_, (byte[])m1_2.Array_);

        r.m3.Write(new ulong[] { 50, 100 }, m2, new ulong[] { 20, 25 }, new ulong[] { 200, 200 });
        MultiDimArray m2_2 = new MultiDimArray(new uint[] { 1024, 1024 }, new byte[1024 * 1024]);
        r.m3.Read(new ulong[] { 0, 0 }, m2_2, new ulong[] { 0, 0 }, new ulong[] { 1024, 1024 });
        ca<uint>(m3.Dims, m2_2.Dims);
        ca<byte>((byte[])m3.Array_, (byte[])m2_2.Array_);

        MultiDimArray m6 = new MultiDimArray(new uint[] { 200, 200 }, new byte[200 * 200]);
        r.m3.Read(new ulong[] { 65, 800 }, m6, new ulong[] { 0, 0 }, new ulong[] { 200, 200 });
        ca<uint>(m4.Dims, m6.Dims);
        ca<byte>((byte[])m4.Array_, (byte[])m6.Array_);

        MultiDimArray m7 = new MultiDimArray(new uint[] { 512, 512 }, new byte[512 * 512]);
        r.m3.Read(new ulong[] { 65, 800 }, m7, new ulong[] { 100, 230 }, new ulong[] { 200, 200 });
        ca<uint>(m5.Dims, m7.Dims);
        ca<byte>((byte[])m5.Array_, (byte[])m7.Array_);
    }

    public void TestAuthentication(string url)
    {
        // Test two different logins
        Dictionary<string, object> cred1 = new Dictionary<string, object>();
        cred1.Add("password", "testpass1");
        testroot r1 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser1", cred1);
        r1.func3(2.2, 3.3);
        RobotRaconteurNode.s.DisconnectService(r1);

        Dictionary<string, object> cred2 = new Dictionary<string, object>();
        cred2.Add("password", "testpass2");
        testroot r2 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser2", cred2);
        r2.func3(2.2, 3.3);
        RobotRaconteurNode.s.DisconnectService(r2);

        // Check an invalid password

        RRAssert.ThrowsException<Exception>(delegate() {
            var r3 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser2", cred1);
            r3.func3(2.2, 3.3);
        });

        bool err2 = false;
        // Check no password
        testroot r4 = null;
        try
        {
            r4 = (testroot)RobotRaconteurNode.s.ConnectService(url);
            r4.func3(2.2, 3.3);
        }
        catch (Exception)
        {
            err2 = true;
            try
            {
                RobotRaconteurNode.s.DisconnectService(r4);
            }
            catch
            {}
        }

        RRAssert.IsTrue(err2);
    }

    public void TestObjectLock(string url)
    {
        // Run a test, check 6 things:
        // 1. Exclusive username lock works as expected (user with lock can access, other user can't)
        // 2. Check that session-level lock works as expected (only one session can access the locked object)
        // 3. Check that "sub-tree" objects lock as expected
        // 5. Object lock release works
        // 6. Object lock override works

        // Log in twice
        Dictionary<string, object> cred1 = new Dictionary<string, object>();
        cred1.Add("password", "testpass1");
        testroot r1 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser1", cred1);

        // Log in again with "testuser1" username
        Dictionary<string, object> cred2 = new Dictionary<string, object>();
        cred2.Add("password", "testpass2");
        testroot r2 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser1", cred1);

        sub1 r1_o = r1.get_o1();
        sub2 r1_o_o2 = r1_o.get_o2_1();
        sub1 r2_o = r2.get_o1();
        sub2 r2_o_o2 = r2_o.get_o2_1();

        testroot r3 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser2", cred2);
        sub1 r3_o = r3.get_o1();
        sub2 r3_o_o2 = r3_o.get_o2_1();

        // Make sure all the objects work
        r1.func3(2.2, 3.3);
        r1_o.d1 = new double[] { 1.0 };
        r1_o_o2.data = "Hello world";
        r2.func3(2.2, 3.3);
        r2_o.d1 = new double[] { 1.0 };
        r2_o_o2.data = "Hello world";
        r3.func3(2.2, 3.3);
        r3_o.d1 = new double[] { 1.0 };
        r3_o_o2.data = "Hello world";

        // Lock the object by username
        RobotRaconteurNode.s.RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags.USER_LOCK);

        // Check that all users that should access the objects can
        r1.func3(2.2, 3.3);
        r1_o.d1 = new double[] { 1.0 };
        r1_o_o2.data = "Hello world";
        r2.func3(2.2, 3.3);
        r2_o.d1 = new double[] { 1.0 };
        r2_o_o2.data = "Hello world";
        r3.func3(2.2, 3.3);

        // Check that objects that shouldn't be able to access the objects can't
        RRAssert.ThrowsException<Exception>(delegate() { r3_o.d1 = new double[] { 1.0 }; });
        RRAssert.ThrowsException<Exception>(delegate() { r3_o_o2.data = "Hello world"; });

        // Unlock and recheck all
        RobotRaconteurNode.s.ReleaseObjectLock(r1_o);

        r1.func3(2.2, 3.3);
        r1_o.d1 = new double[] { 1.0 };
        r1_o_o2.data = "Hello world";
        r2.func3(2.2, 3.3);
        r2_o.d1 = new double[] { 1.0 };
        r2_o_o2.data = "Hello world";
        r3.func3(2.2, 3.3);
        r3_o.d1 = new double[] { 1.0 };
        r3_o_o2.data = "Hello world";

        // Relock, test that the lock is active, and then close the connection.  The lock should release.  The
        // second session is closed first, and should not release the lock.
        RobotRaconteurNode.s.RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags.USER_LOCK);
        RRAssert.ThrowsException<Exception>(delegate() { r3_o.d1 = new double[] { 1.0 }; });

        r2_o.d1 = new double[] { 1.0 };
        RobotRaconteurNode.s.DisconnectService(r2);

        // Object still should be locked
        RRAssert.ThrowsException<Exception>(delegate() { r3_o.d1 = new double[] { 1.0 }; });

        // Now close the session and lock should be released
        RobotRaconteurNode.s.DisconnectService(r1);
        r3_o.d1 = new double[] { 1.0 };

        // Reconnect the first two test sessions
        r1 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser1", cred1);
        r2 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testuser2", cred2);
        r1_o = r1.get_o1();
        r1_o_o2 = r1_o.get_o2_1();
        r2_o = r2.get_o1();
        r2_o_o2 = r2_o.get_o2_1();

        // Test the exclusive client lock
        RobotRaconteurNode.s.RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags.CLIENT_LOCK);
        r1_o.d1 = new double[] { 1.0 };
        RRAssert.ThrowsException<Exception>(delegate() { r2_o.d1 = new double[] { 1.0 }; });
        RRAssert.ThrowsException<Exception>(delegate() { r3_o.d1 = new double[] { 1.0 }; });

        // Test the lock override by testsuperpass
        Dictionary<string, object> cred5 = new Dictionary<string, object>();
        cred5.Add("password", "superpass1");
        testroot r5 = (testroot)RobotRaconteurNode.s.ConnectService(url, "testsuperuser", cred5);
        sub1 r5_o = r5.get_o1();
        RobotRaconteurNode.s.ReleaseObjectLock(r5_o);

        // Make sure the lock is released
        r2_o.d1 = new double[] { 1.0 };
        r3_o.d1 = new double[] { 1.0 };

        // Close all connections
        RobotRaconteurNode.s.DisconnectService(r1);
        RobotRaconteurNode.s.DisconnectService(r2);
        RobotRaconteurNode.s.DisconnectService(r3);
        RobotRaconteurNode.s.DisconnectService(r5);
    }

    public void TestMonitorLock(string url)
    {

        // The monitor lock acquires an exclusive sock to a single thread on the client.
        // This lock is for a single thread, and works for all clients and the service if
        // it is checking for monitor locks.

        testroot r1 = (testroot)RobotRaconteurNode.s.ConnectService(url);
        testroot r2 = (testroot)RobotRaconteurNode.s.ConnectService(url);

        sub1 r1_o = r1.get_o1();
        sub2 r1_o_o2 = r1_o.get_o2_1();
        sub1 r2_o = r2.get_o1();
        sub2 r2_o_o2 = r2_o.get_o2_1();

        // Make sure everything works
        r1.func3(2.2, 3.3);
        r1_o.d1 = new double[] { 1.0 };
        r1_o_o2.data = "Hello world";
        r2.func3(2.2, 3.3);
        r2_o.d1 = new double[] { 1.0 };
        r2_o_o2.data = "Hello world";

        // Make sure that locks work as expected

        bool t1 = false;
        bool t2 = false;

        System.Threading.AutoResetEvent e1 = new System.Threading.AutoResetEvent(false);
        // AutoResetEvent e2 = new AutoResetEvent(false);

        bool threaderr = false;
            Thread t = new Thread(delegate ()
            {
                try
                {
                    r2.func3(2.2, 3.3);
            r2_o_o2.data = "Hello world";
            // RRAssert.ThrowsException<ObjectLockedException>(delegate() { r2_o.d1 = new double[] { 0.0 }; });
            e1.Set();
            RobotRaconteurNode.s.MonitorEnter(r2_o);
            RRAssert.IsFalse(t1);
            t2 = true;

            r2_o.d1 = new double[] { 0.0 };
            RobotRaconteurNode.s.MonitorExit(r2_o);
    }
    catch
    {
        threaderr = true;
    }
});

RobotRaconteurNode.s.MonitorEnter(r1_o);
t.Start();
t1 = true;
RRAssert.IsTrue(e1.WaitOne(1000));
Thread.Sleep(10);
r1.func3(2.2, 3.3);
r1_o.d1 = new double[] { 0.0 };
RRAssert.IsFalse(t2);
t1 = false;
RobotRaconteurNode.s.MonitorExit(r1_o);

t.Join();

RobotRaconteurNode.s.DisconnectService(r1);
RobotRaconteurNode.s.DisconnectService(r2);

RRAssert.IsFalse(threaderr);

}

public void TestAsync(string url)
{
    var t1 = TestAsync1(url);
    t1.Wait(10000);
    RRAssert.IsFalse(t1.IsFaulted);
}

async Task TestAsync1(string url)
{
    Dictionary<string, object> cred1 = new Dictionary<string, object>();
    cred1.Add("password", "testpass1");
    var r = await RobotRaconteurNode.s.AsyncConnectService(url, "testuser1", cred1, null, null);

    async_testroot r1 = (async_testroot)r;
    var v1 = await r1.async_get_d2();

    ca<double>(v1, TestVectors.Get<double>("ts1.d2.to_client"));
    double[] d2_dat = TestVectors.Get<double>("ts1.d2.to_service");

    await r1.async_set_d2(d2_dat);

    await r1.async_func1();

    var v2 = await r1.async_func3(2, 3.45);
    RRAssert.AreEqual(v2, 5.45);

    await RRAssert.ThrowsExceptionAsync<Exception>(async delegate() { await r1.async_func_errtest(); });

    var o1 = await r1.async_get_o1();

    RRAssert.AreNotEqual(o1, null);
    double[] d1 = o1.d1;

    var r2 = (testroot)r;
    var e1 = await r2.p1.AsyncConnect(-1);

    await e1.AsyncSendPacket(new double[] { 1, 2, 3, 4 });

    await e1.AsyncClose();

    var w1 = await r2.w1.AsyncConnect();

    w1.OutValue = new double[] { 0.0 };
    await w1.AsyncClose();

    await RobotRaconteurNode.s.AsyncRequestObjectLock(r, RobotRaconteurObjectLockFlags.CLIENT_LOCK);

    await RobotRaconteurNode.s.AsyncReleaseObjectLock(r);

    await RobotRaconteurNode.s.AsyncDisconnectService(r);
}
}
}

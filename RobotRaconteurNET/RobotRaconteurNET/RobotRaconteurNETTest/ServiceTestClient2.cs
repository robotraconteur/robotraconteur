using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using com.robotraconteur.testing.TestService1;
using com.robotraconteur.testing.TestService2;
using com.robotraconteur.testing.TestService3;
using RobotRaconteur;
using System.Threading;
using System.IO;

namespace RobotRaconteurNETTest
{
    public class ServiceTestClient2
    {

        public void ConnectService(string url)
        {
            r = (testroot3)RobotRaconteurNode.s.ConnectService(url);
        }

        public void DisconnectService()
        {
            RobotRaconteurNode.s.DisconnectService(r);
        }

        
        testroot3 r;

        public void RunFullTest(string url)
        {
            ConnectService(url);

            if (r.testenum1_prop != testenum1.anothervalue) throw new Exception();
            r.testenum1_prop = testenum1.hexval1;

            r.get_o4();

            TestWirePeekPoke();

            AsyncTestWirePeekPoke();

            TestEnums();

            TestCStructs();

            TestGenerators();

            TestMemories();

            TestAStructs();

            TestAStructMemories();

            DisconnectService();
        }

        public void TestWirePeekPoke()
        {
            TimeSpec ts;
            int v = r.peekwire.PeekInValue(out ts);
            if (v != 56295674) throw new Exception();
            
            TimeSpec ts2;
            r.pokewire.PokeOutValue(75738265);
            int v2 = r.pokewire.PeekOutValue(out ts2);
            if (v2 != 75738265) throw new Exception();

            var w = r.pokewire.Connect();
            for (int i=0; i<3; i++)
            {
                w.OutValue = 8638356;
            }

            Thread.Sleep(100);

            TimeSpec ts3;
            var v3 = r.pokewire.PeekOutValue(out ts3);
            if (v3 != 8638356) throw new Exception();

        }

        void TestAsync_err(Exception exp)
        {
            lock (async_err_lock)
            {
                async_err = exp;
                async_wait.Set();
            }
        }

        System.Threading.AutoResetEvent async_wait = new System.Threading.AutoResetEvent(false);
        object async_err_lock = new object();
        Exception async_err = null;

        public void AsyncTestWirePeekPoke()
        {
            r.peekwire.AsyncPeekInValue(AsyncTestWirePeekPoke1);

            async_wait.WaitOne();

            lock (async_err_lock)
            {
                if (async_err != null)
                    throw async_err;
            }
        }

        public void AsyncTestWirePeekPoke1(int value, TimeSpec ts, Exception err)
        {
            if (err != null)
            {
                TestAsync_err(err);
                return;
            }

            if (value != 56295674)
            {
                TestAsync_err(new Exception());
                return;
            }

            r.pokewire.AsyncPokeOutValue(75738261, AsyncTestWirePeekPoke2);

        }

        public void AsyncTestWirePeekPoke2(Exception err)
        {
            if (err != null)
            {
                TestAsync_err(err);
                return;
            }

            r.pokewire.AsyncPeekOutValue(AsyncTestWirePeekPoke3);

        }

        public void AsyncTestWirePeekPoke3(int value, TimeSpec ts, Exception err)
        {
            if (err != null)
            {
                TestAsync_err(err);
                return;
            }

            if (value != 75738261)
            {
                TestAsync_err(new Exception());
                return;
            }

            async_wait.Set();

        }

        public void TestEnums()
        {
            if (r.testenum1_prop != testenum1.anothervalue)
                throw new Exception("");

            r.testenum1_prop = testenum1.hexval1;

        }

        public void TestCStructs()
        {
            /*var s1 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(ref s1, 563921043);
            ServiceTest2_cstruct.verify_testcstruct1(ref s1, 563921043);

            var s1_m = RobotRaconteurNode.s.PackCStructureToArray(ref s1);
            var s1_1 = RobotRaconteurNode.s.UnpackCStructureFromArray<testcstruct1>(s1_m);
            ServiceTest2_cstruct.verify_testcstruct1(ref s1_1, 563921043);

            var s2 = ServiceTest2_cstruct.fill_teststruct3(858362);
            ServiceTest2_cstruct.verify_teststruct3(s2, 858362);
            var s2_m = RobotRaconteurNode.s.PackStructure(s2);
            var s2_1 = RobotRaconteurNode.s.UnpackStructure<teststruct3>(s2_m);
            ServiceTest2_cstruct.verify_teststruct3(s2_1, 858362);*/

            var p1 = r.testcstruct1_prop;
            ServiceTest2_cstruct.verify_testcstruct1(ref p1, 563921043);
            var p2 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(ref p2, 85932659);
            r.testcstruct1_prop = p2;

            var f1 = r.testcstruct1_func2();
            ServiceTest2_cstruct.verify_testcstruct1(ref f1, 95836295);
            var f2 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(ref f2, 29546592);
            r.testcstruct1_func1(f2);

            ServiceTest2_cstruct.verify_teststruct3(r.teststruct3_prop, 16483675);
            r.teststruct3_prop = (ServiceTest2_cstruct.fill_teststruct3(858362));

        }

        public void TestGenerators()
        {
            var g = r.gen_func1();
            var res=g.NextAll();
            Console.WriteLine(res);

            var g2 = r.gen_func1();
            g2.Next();
            g2.Abort();
            try
            {
                g2.Next();
            }
            catch (OperationAbortedException)
            {
                Console.WriteLine("Operation aborted caught");
            }

            var g3 = r.gen_func1();
            g3.Next();
            g3.Close();
            try
            {
                g3.Next();
            }
            catch (StopIterationException)
            {
                Console.WriteLine("Stop iteration caught");
            }

            var gen2 = r.gen_func4();
            gen2.Next(new byte[] { 2, 3, 4 });
            gen2.Close();
            try
            {
                gen2.Next(new byte[] { 2, 3, 4 });
            }
            catch (StopIterationException)
            {
                Console.WriteLine("Stop iteration caught");
            }
        }

        public void TestMemories()
        {
            test_m1();
            test_m2();
        }

        public void test_m1()
        {
            testcstruct2[] o1 = new testcstruct2[32];

            for (uint i = 0; i < o1.Length; i++)
            {
                ServiceTest2_cstruct.fill_testcstruct2(ref o1[i], 59174 + i);
            }

            if (r.cstruct_m1.Length != 1024) throw new Exception("");

            r.cstruct_m1.Write(52, o1, 3, 17);

            testcstruct2[] o2 = new testcstruct2[32];

            r.cstruct_m1.Read(53, o2, 2, 16);

            for (uint i = 2; i < 16; i++)
            {
                ServiceTest2_cstruct.verify_testcstruct2(ref o2[i], 59174 + i + 2);
            }
        }

        public void test_m2()
        {
            CStructureMultiDimArray s = new CStructureMultiDimArray(new int[] { 3, 3 }, new testcstruct2[9]);

            for (uint i = 0; i < s.cstruct_array.Length; i++)
            {
                ServiceTest2_cstruct.fill_testcstruct2(ref ((testcstruct2[])s.cstruct_array)[i], 75721 + i);
            }

            r.cstruct_m2.Write(new ulong[] { 0, 0 }, s, new ulong[] { 0, 0 }, new ulong[] { 3, 3 });

            CStructureMultiDimArray s2 = new CStructureMultiDimArray(new int[] { 3, 3 }, new testcstruct2[9]);

            r.cstruct_m2.Read(new ulong[] { 0, 0 }, s2, new ulong[] { 0, 0 }, new ulong[] { 3, 3 });

            for (uint i = 0; i < s2.cstruct_array.Length; i++)
            {
                ServiceTest2_cstruct.verify_testcstruct2(ref ((testcstruct2[])s2.cstruct_array)[i], 75721 + i);
            }
        }

        public void TestAStructs()
        {
            var a1 = new transform();
            ServiceTest2_cstruct.fill_transform(ref a1, 3956378);
            r.testastruct1 = a1.translation;

            var a1_1 = new transform();
            a1_1.rotation = a1.rotation;
            a1_1.translation = r.testastruct1;
            var a1_2 = new transform();
            ServiceTest2_cstruct.fill_transform(ref a1_2, 74637);
            a1_1.rotation = a1_2.rotation;
            ServiceTest2_cstruct.verify_transform(ref a1_1, 74637);

            var a2 = new transform();
            ServiceTest2_cstruct.fill_transform(ref a2, 827635);
            r.testastruct2=a2;

            transform a2_1 = r.testastruct2;
            ServiceTest2_cstruct.verify_transform(ref a2_1, 1294);

            r.testastruct3=ServiceTest2_cstruct.fill_transform_array(6, 19274);
            ServiceTest2_cstruct.verify_transform_array(r.testastruct3, 8, 837512);

            r.testastruct4 = ServiceTest2_cstruct.fill_transform_multidimarray(5, 2, 6385);
            ServiceTest2_cstruct.verify_transform_multidimarray(r.testastruct4, 7, 2, 66134);

            r.testastruct5=ServiceTest2_cstruct.fill_transform_multidimarray(3, 2, 7732);
            ServiceTest2_cstruct.verify_transform_multidimarray(r.testastruct5, 3, 2, 773142);

        }

        public void TestAStructMemories()
        {
            test_astruct_m1();
            test_astruct_m2();
        }

        public void test_astruct_m1()
        {
            var s = new transform[32];
            for (uint i=0; i< s.Length; i++)
                ServiceTest2_cstruct.fill_transform(ref s[i], 79174 + i);

            if (r.astruct_m1.Length != 512) throw new Exception();
            r.astruct_m1.Write(23, s, 3, 21);

            var s2 = new transform[32];
            r.astruct_m1.Read(24, s2, 2, 18);

            for (uint i = 2; i<18; i++)
            {
                ServiceTest2_cstruct.verify_transform(ref s2[i], 79174 + i + 2);
            }
        }

        public void test_astruct_m2()
        {
            var s = new AStructureMultiDimArray(new int[] { 3, 3 }, new transform[9]);
            var s_array = (transform[])s.astruct_array;
            for (uint i = 0; i < s.astruct_array.Length; i++)
                ServiceTest2_cstruct.fill_transform(ref s_array[i], 15721 + i);

            r.astruct_m2.Write(new ulong[] { 0, 0 }, s, new ulong[] { 0, 0 }, new ulong[] { 3, 3 });

            var s2 = new AStructureMultiDimArray(new int[] { 3, 3 }, new transform[9]);
            r.astruct_m2.Read(new ulong[] { 0, 0 }, s2, new ulong[] { 0, 0 }, new ulong[] { 3, 3 });

            var s2_array = (transform[])s2.astruct_array;
            for (uint i = 0; i < s2.astruct_array.Length; i++)
                ServiceTest2_cstruct.fill_transform(ref s2_array[i], 15721 + i);

        }
    }
    
}
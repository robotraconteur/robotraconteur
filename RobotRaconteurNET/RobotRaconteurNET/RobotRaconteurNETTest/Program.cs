using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using System.IO;

using com.robotraconteur.testing.TestService1;
using com.robotraconteur.testing.TestService2;
using com.robotraconteur.testing.TestService3;
using com.robotraconteur.testing.TestService5;

using System.Threading.Tasks;

namespace RobotRaconteurNETTest
{
class Program
{
    static void Main(string[] args)
    {
        // string path =
        // @"C:\Users\wasonj\Documents\RobotRaconteur2\bin_devel\out_debug\NET\Native\RobotRaconteurNETNative.dll";

        // Environment.SetEnvironmentVariable("PATH", Path.GetDirectoryName(path) + ";" +
        // Environment.GetEnvironmentVariable("PATH"));

        string exepath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
        MultiDimArrayTest.testdatapath = System.IO.Path.Combine(exepath, System.IO.Path.Combine("..", "testdata"));

        RobotRaconteurNode.s.SetExceptionHandler(delegate(Exception e) { Console.WriteLine(e.ToString()); });

        string command = "loopback";

        if (args.Length >= 1)
        {
            command = args[0];
        }

        if (command == "loopback")
        {

            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            TcpTransport t = new TcpTransport();
            t.StartServer(2323);
            t.EnableNodeDiscoveryListening();
            t.EnableNodeAnnounce();

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            // MultiDimArrayTest.Test();

            RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
            sup.RegisterServices(t);

            int count = 1;

            if (args.Length >= 2)
            {
                count = int.Parse(args[1]);
            }

            for (int i = 0; i < count; i++)
            {

                ServiceTestClient c = new ServiceTestClient();
                c.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService",
                              "tcp://localhost:2323/{0}/RobotRaconteurTestService_auth");
                // System.Threading.Thread.Sleep(100);
            }

            /* c = new ServiceTestClient();
             c.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService",
             "tcp://localhost:2323/{0}/RobotRaconteurTestService_auth"); System.Threading.Thread.Sleep(1000);*/

            /*System.Threading.Thread.Sleep(10000);

            ServiceInfo2[] services = RobotRaconteurNode.s.FindServiceByType("RobotRaconteurTestService.testroot", new
            string[] { "tcp" });*/

            try
            {
                object o = RobotRaconteurNode.s.ConnectService("tcp://localhost:2323/{0}/RobotRaconteurTestService");
            }
            catch
            {}

            // System.Threading.Thread.Sleep(17000);

            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "loopback2")
        {

            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            using (var setup = new ServerNodeSetup("com.robotraconteur.testing.TestService2", 4565,
                                                   RobotRaconteurNodeSetupFlags.ENABLE_TCP_TRANSPORT |
                                                       RobotRaconteurNodeSetupFlags.TCP_TRANSPORT_START_SERVER))
            {
                // MultiDimArrayTest.Test();

                RobotRaconteurTestServiceSupport2 sup = new RobotRaconteurTestServiceSupport2();
                sup.RegisterServices(setup.TcpTransport);

                ServiceTestClient2 c = new ServiceTestClient2();
                c.RunFullTest("rr+tcp://localhost:4565/?service=RobotRaconteurTestService2");
            }
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "loopback3")
        {

            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            using (var setup = new ServerNodeSetup("com.robotraconteur.testing.TestService3", 4567,
                                                   RobotRaconteurNodeSetupFlags.ENABLE_TCP_TRANSPORT |
                                                       RobotRaconteurNodeSetupFlags.TCP_TRANSPORT_START_SERVER))
            {

                RobotRaconteurTestServiceSupport3 sup = new RobotRaconteurTestServiceSupport3();
                sup.RegisterServices();

                ServiceTestClient3 c = new ServiceTestClient3();
                c.RunFullTest("rr+tcp://localhost:4567/?service=RobotRaconteurTestService3");
            }
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "client")
        {

            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string url = args[1];
            string url_auth = args[2];

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            HardwareTransport t4 = new HardwareTransport();
            RobotRaconteurNode.s.RegisterTransport(t4);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            // MultiDimArrayTest.Test();

            RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
            sup.RegisterServices(t);

            int count = 1;

            if (args.Length >= 4)
            {
                count = int.Parse(args[3]);
            }

            for (int i = 0; i < count; i++)
            {

                ServiceTestClient c = new ServiceTestClient();
                c.RunFullTest(url, url_auth);
                // System.Threading.Thread.Sleep(100);
            }

            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "client2")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string url = args[1];

            TcpTransport t = new TcpTransport();

            t.EnableNodeDiscoveryListening();

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService3.com__robotraconteur__testing__TestService3Factory());

            // MultiDimArrayTest.Test();

            RobotRaconteurTestServiceSupport2 sup = new RobotRaconteurTestServiceSupport2();
            sup.RegisterServices(t);

            ServiceTestClient2 c = new ServiceTestClient2();
            c.RunFullTest(url);

            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "client3")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string url = args[1];

            TcpTransport t = new TcpTransport();

            t.EnableNodeDiscoveryListening();

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService5.com__robotraconteur__testing__TestService5Factory());

            ServiceTestClient3 c = new ServiceTestClient3();
            c.RunFullTest(url);

            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test completed");
            return;
        }

        if (command == "server")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            int port;
            if (args[1] == "sharer")
            {
                port = -1;
            }
            else
            {
                port = Int32.Parse(args[1]);
            }
            string name = args[2];

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);
            t2.StartServerAsNodeName(name);

            TcpTransport t = new TcpTransport();
            t.EnableNodeAnnounce();
            if (port > 0)
            {
                t.StartServer(port);
            }
            else
            {
                t.StartServerUsingPortSharer();
            }

            try
            {
                t.LoadTlsNodeCertificate();
            }
            catch (Exception)
            {
                Console.WriteLine("warning: Could not load local node certificate");
            }

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService3.com__robotraconteur__testing__TestService3Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService5.com__robotraconteur__testing__TestService5Factory());

            RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
            sup.RegisterServices(t);

            RobotRaconteurTestServiceSupport2 sup2 = new RobotRaconteurTestServiceSupport2();
            sup2.RegisterServices(t);

            RobotRaconteurTestServiceSupport2 sup3 = new RobotRaconteurTestServiceSupport2();
            sup3.RegisterServices(t);

            Console.WriteLine("Server started, press enter to quit");
            Console.ReadLine();
            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test complete, no error detected");
            return;
        }

        if (command == "findservicebytype")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string type = args[1];
            string[] tschemes = args[2].Split(new char[] { ',' });

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            System.Threading.Thread.Sleep(6000);

            ServiceInfo2[] ret = RobotRaconteurNode.s.FindServiceByType(type, tschemes);

            foreach (ServiceInfo2 r in ret)
            {
                print_ServiceInfo2(r);
            }

            var t1 = RobotRaconteurNode.s.AsyncFindServiceByType(type, tschemes);
            t1.Wait();
            var ret2 = t1.Result;
            {
                foreach (ServiceInfo2 r in ret2)
                {
                    print_ServiceInfo2(r);
                }
            }

            System.Threading.Thread.Sleep(10000);
            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "findnodebyid")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            NodeID id = new NodeID(args[1]);
            string[] tschemes = args[2].Split(new char[] { ',' });

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            System.Threading.Thread.Sleep(6000);

            NodeInfo2[] ret = RobotRaconteurNode.s.FindNodeByID(id, tschemes);

            foreach (NodeInfo2 r in ret)
            {
                print_NodeInfo2(r);
            }

            var ts1 = RobotRaconteurNode.s.AsyncFindNodeByID(id, tschemes);
            ts1.Wait();
            var ret2 = ts1.Result;
            {
                foreach (NodeInfo2 r in ret2)
                {
                    print_NodeInfo2(r);
                }
            }

            System.Threading.Thread.Sleep(10000);
            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "findnodebyname")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string name = args[1];
            string[] tschemes = args[2].Split(new char[] { ',' });

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            System.Threading.Thread.Sleep(6000);

            NodeInfo2[] ret = RobotRaconteurNode.s.FindNodeByName(name, tschemes);

            foreach (NodeInfo2 r in ret)
            {
                print_NodeInfo2(r);
            }

            var ts1 = RobotRaconteurNode.s.AsyncFindNodeByName(name, tschemes);
            ts1.Wait();
            var ret2 = ts1.Result;
            {
                foreach (NodeInfo2 r in ret2)
                {
                    print_NodeInfo2(r);
                }
            }

            System.Threading.Thread.Sleep(10000);
            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "stresstestclient")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string url1 = args[1];

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            servicetest_count = 0;
            servicetest_keepgoing = true;

            object obj = RobotRaconteurNode.s.ConnectService(url1);
            async_testroot o = (async_testroot)obj;
            testroot o2 = (testroot)obj;
            o.async_func3(1, 2).ContinueWith(ts1 => servicetest2(o, ts1));

            Pipe<double>.PipeEndpoint p = o2.broadcastpipe.Connect(-1);
            p.PacketReceivedEvent += servicetest7;
            Wire<double>.WireConnection w = o2.broadcastwire.Connect();
            RobotRaconteur.Timer tt =
                RobotRaconteurNode.s.CreateTimer(40, delegate(TimerEvent ev) { servicetest5(p, w, ev); });
            tt.Start();

            Console.WriteLine("Press enter to quit");
            Console.ReadLine();
            servicetest_keepgoing = false;
            tt.Stop();
            RobotRaconteurNode.s.Shutdown();

            return;
        }

        if (command == "latencytestclient" || command == "latencytestclient2")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            string url1 = args[1];

            LocalTransport t2 = new LocalTransport();
            RobotRaconteurNode.s.RegisterTransport(t2);

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            servicetest_count = 0;
            servicetest_keepgoing = true;

            object obj = RobotRaconteurNode.s.ConnectService(url1);
            async_testroot o = (async_testroot)obj;
            testroot o2 = (testroot)obj;

            var o3 = o2.get_o1();

            int iters = 100000;

            var d = new double[10];

            DateTime start;
            DateTime end;

            if (command == "latencytestclient")
            {
                start = DateTime.UtcNow;
                for (int i = 0; i < iters; i++)
                {
                    o3.d1 = d;
                }
                end = DateTime.UtcNow;
            }
            else
            {
                start = DateTime.UtcNow;
                for (int i = 0; i < iters; i++)
                {
                    var dummy = o2.struct1;
                }
                end = DateTime.UtcNow;
            }

            var diff = (end - start).Ticks / (TimeSpan.TicksPerMillisecond / 1000);

            double period = ((double)diff) / ((double)iters);

            Console.WriteLine("Period = {0}", period);

            RobotRaconteurNode.s.Shutdown();

            return;
        }

        if (command == "peeridentity")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            if (args.Length < 2)
            {
                Console.WriteLine("Usage for peeridentity:  RobotRaconteurTest peeridentity url [nodeid]");
                return;
            }

            var url1 = args[1];
            Console.WriteLine(url1);
            var c = new TcpTransport();

            if (args.Length > 2)
            {
                var nodeid = args[2];

                var id = new NodeID(nodeid);

                RobotRaconteurNode.s.NodeID = id;

                try
                {
                    c.LoadTlsNodeCertificate();
                }
                catch (Exception)
                {
                    Console.WriteLine("warning: Could not load local node certificate");
                }
            }

            Console.WriteLine(RobotRaconteurNode.s.NodeID);
            var c2 = new LocalTransport();

            var c5 = new HardwareTransport();

            RobotRaconteurNode.s.RegisterTransport(c);
            RobotRaconteurNode.s.RegisterTransport(c2);

            RobotRaconteurNode.s.RegisterTransport(c5);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            var oo = (testroot)RobotRaconteurNode.s.ConnectService(url1);
            oo.func3(1.0, 2.3);

            if (c.IsTransportConnectionSecure(oo))
            {
                Console.WriteLine("Connection is secure");

                if (c.IsSecurePeerIdentityVerified(oo))
                {
                    Console.WriteLine("Peer identity is verified: " + c.GetSecurePeerIdentity(oo));
                }
                else
                {
                    Console.WriteLine("Peer identity is not verified");
                }
            }
            else
            {
                Console.WriteLine("Connection is not secure");
            }

            RobotRaconteurNode.s.Shutdown();

            Console.WriteLine("Test completed, no errors detected");
            return;
        }

        if (command == "multidimarraytest")
        {
            MultiDimArrayTest.Test();
            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "subscribertest")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            if (args.Length < 2)
            {
                Console.WriteLine("Usage for subscribertest:  RobotRaconteurTest subscribertest servicetype");
                return;
            }

            var servicetype = args[1];

            LocalTransport t2 = new LocalTransport();
            t2.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t2);

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            HardwareTransport t3 = new HardwareTransport();
            RobotRaconteurNode.s.RegisterTransport(t3);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            var subscription = RobotRaconteurNode.s.SubscribeServiceByType(new string[] { servicetype });

            subscription.ClientConnected += delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
            {
                Console.WriteLine("Client connected: " + d.NodeID.ToString() + ", " + d.ServiceName);
                testroot e1 = (testroot)e;
                Console.WriteLine("d1 = " + e1.d1);
            };

            subscription.ClientDisconnected += delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
            {
                Console.WriteLine("Client disconnected: " + d.NodeID.ToString() + ", " + d.ServiceName);
            };

            var wire_subscription = subscription.SubscribeWire<double>("broadcastwire");
            wire_subscription.WireValueChanged += delegate(WireSubscription<double> c, double d, TimeSpec e) {
                // Console.WriteLine("Wire value changed: " + d);
            };

            var pipe_subscription = subscription.SubscribePipe<double>("broadcastpipe");
            pipe_subscription.PipePacketReceived += delegate(PipeSubscription<double> c)
            {
                double val;
                while (c.TryReceivePacket(out val))
                {
                    Console.WriteLine("Received pipe packet: " + val);
                }
            };

            System.Threading.Thread.Sleep(6000);

            var connected_clients = subscription.GetConnectedClients();

            foreach (var c in connected_clients)
            {
                Console.WriteLine("Client: " + c.Key.NodeID + ", " + c.Key.ServiceName);
            }

            TimeSpec w1_time = null;
            double w1_value;
            var w1_res = wire_subscription.TryGetInValue(out w1_value);

            if (w1_res)
            {
                Console.WriteLine("Got broadcastwire value: " + w1_value + " " + w1_time?.seconds);
            }

            Console.WriteLine("Waiting for services...");

            Console.ReadLine();

            RobotRaconteurNode.s.Shutdown();

            return;
        }

        if (command == "subscriberurltest")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            if (args.Length < 2)
            {
                Console.WriteLine("Usage for subscriberurltest:  RobotRaconteurTest subscriberurltest url");
                return;
            }

            var url = args[1];

            LocalTransport t2 = new LocalTransport();
            t2.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t2);

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            HardwareTransport t3 = new HardwareTransport();
            RobotRaconteurNode.s.RegisterTransport(t3);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            var subscription = RobotRaconteurNode.s.SubscribeService(url);

            subscription.ClientConnected += delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
            {
                Console.WriteLine("Client connected: " + d.NodeID.ToString() + ", " + d.ServiceName);
                testroot e1 = (testroot)e;
                Console.WriteLine("d1 = " + e1.d1);
            };

            subscription.ClientDisconnected += delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
            {
                Console.WriteLine("Client disconnected: " + d.NodeID.ToString() + ", " + d.ServiceName);
            };

            subscription.ClientConnectFailed +=
                delegate(ServiceSubscription c, ServiceSubscriptionClientID d, string[] url2, Exception err)
            {
                Console.WriteLine("Client connect failed: " + d.NodeID.ToString() + " url: " + String.Join(",", url2) +
                                  err.ToString());
            };

            subscription.AsyncGetDefaultClient(1000).ContinueWith(delegate(Task<object> res) {
                if (res.IsFaulted)
                {
                    Console.WriteLine("AsyncGetDefaultClient failed");
                }
                else if (res.Result == null)
                {
                    Console.WriteLine("AsyncGetDefaultClient returned null");
                }
                else
                {
                    Console.WriteLine($"AsyncGetDefaultClient successful: {res.Result}");
                }
            });
            var client2 = subscription.GetDefaultClientWait(6000);
            object client3;
            var try_res = subscription.TryGetDefaultClientWait(out client3, 6000);
            Console.WriteLine($"try_res = {try_res}");

            var connected_clients = subscription.GetConnectedClients();

            foreach (var c in connected_clients)
            {
                Console.WriteLine("Client: " + c.Key.NodeID + ", " + c.Key.ServiceName);
            }

            try
            {
                Console.WriteLine(((testroot)subscription.GetDefaultClient()).d1);
            }
            catch (Exception)
            {
                Console.WriteLine("Client not connected");
            }

            object client1;
            subscription.TryGetDefaultClient(out client1);

            Console.WriteLine("Waiting for services...");

            Console.ReadLine();

            RobotRaconteurNode.s.Shutdown();

            return;
        }

        if (command == "subscriberfiltertest")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            if (args.Length < 2)
            {
                throw new Exception(
                    "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest servicetype");
            }

            var servicetype = args[1];

            var f = new ServiceSubscriptionFilter();

            if (args.Length >= 3)
            {
                var subcommand = args[2];

                if (subcommand == "nodeid")
                {
                    if (args.Length < 4)
                    {
                        throw new Exception(
                            "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeid <nodeid>");
                    }

                    var n = new ServiceSubscriptionFilterNode();
                    n.NodeID = new NodeID(args[3]);
                    f.Nodes = new ServiceSubscriptionFilterNode[] { n };
                }

                else if (subcommand == "nodename")
                {
                    if (args.Length < 4)
                    {
                        throw new Exception(
                            "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodename <nodename>");
                    }

                    var n = new ServiceSubscriptionFilterNode();
                    n.NodeName = args[3];
                    f.Nodes = new ServiceSubscriptionFilterNode[] { n };
                }
                else if (subcommand == "nodeidscheme")
                {
                    if (args.Length < 5)
                    {
                        throw new Exception(
                            "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeidscheme <nodeid> <schemes>");
                    }

                    var n = new ServiceSubscriptionFilterNode();
                    n.NodeID = new NodeID(args[3]);
                    f.Nodes = new ServiceSubscriptionFilterNode[] { n };
                    f.TransportSchemes = args[4].Split(new char[] { ',' });
                }
                else if (subcommand == "nodeidauth")
                {
                    if (args.Length < 6)
                    {
                        throw new Exception(
                            "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest nodeidauth <nodeid> <username> <password>");
                    }

                    var n = new ServiceSubscriptionFilterNode();
                    n.NodeID = new NodeID(args[3]);
                    n.Username = args[4];
                    n.Credentials = new Dictionary<string, object>() { { "password", args[5] } };
                    f.Nodes = new ServiceSubscriptionFilterNode[] { n };
                }
                else if (subcommand == "servicename")
                {
                    if (args.Length < 4)
                    {
                        throw new Exception(
                            "Usage for subscriberfiltertest:  RobotRaconteurTest subscriberfiltertest servicename <servicename>");
                    }

                    var n = new ServiceSubscriptionFilterNode();
                    f.ServiceNames = new string[] { args[3] };
                }
                else if (subcommand == "predicate")
                {
                    f.Predicate = delegate(ServiceInfo2 info)
                    {
                        Console.WriteLine("Predicate: " + info.NodeName);
                        return info.NodeName == "testprog";
                    };
                }
                else
                {
                    throw new Exception("Unknown subscriberfiltertest command");
                }

                LocalTransport t2 = new LocalTransport();
                t2.EnableNodeDiscoveryListening();
                RobotRaconteurNode.s.RegisterTransport(t2);

                TcpTransport t = new TcpTransport();
                t.EnableNodeDiscoveryListening();
                RobotRaconteurNode.s.RegisterTransport(t);

                HardwareTransport t3 = new HardwareTransport();
                RobotRaconteurNode.s.RegisterTransport(t3);

                RobotRaconteurNode.s.RegisterServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s.RegisterServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                var subscription = RobotRaconteurNode.s.SubscribeServiceByType(new string[] { servicetype }, f);

                subscription.ClientConnected += delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
                {
                    Console.WriteLine("Client connected: " + d.NodeID.ToString() + ", " + d.ServiceName);
                    testroot e1 = (testroot)e;
                    Console.WriteLine("d1 = " + e1.d1);
                };

                subscription.ClientDisconnected +=
                    delegate(ServiceSubscription c, ServiceSubscriptionClientID d, object e)
                {
                    Console.WriteLine("Client disconnected: " + d.NodeID.ToString() + ", " + d.ServiceName);
                };

                Console.ReadLine();

                RobotRaconteurNode.s.Shutdown();

                return;
            }

            return;
        }

        if (command == "serviceinfo2subscribertest")
        {
            RobotRaconteurNode.s.SetLogLevelFromEnvVariable();

            if (args.Length < 2)
            {
                Console.WriteLine("Usage for subscribertest:  RobotRaconteurTest subscribertest servicetype");
                return;
            }

            var servicetype = args[1];

            LocalTransport t2 = new LocalTransport();
            t2.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t2);

            TcpTransport t = new TcpTransport();
            t.EnableNodeDiscoveryListening();
            RobotRaconteurNode.s.RegisterTransport(t);

            HardwareTransport t3 = new HardwareTransport();
            RobotRaconteurNode.s.RegisterTransport(t3);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            var subscription = RobotRaconteurNode.s.SubscribeServiceInfo2(new string[] { servicetype });
            subscription.ServiceDetected +=
                delegate(ServiceInfo2Subscription sub, ServiceSubscriptionClientID id, ServiceInfo2 info)
            {
                Console.WriteLine("Service detected: " + info.NodeID.ToString() + ", " + info.Name);
            };

            subscription.ServiceLost +=
                delegate(ServiceInfo2Subscription sub, ServiceSubscriptionClientID id, ServiceInfo2 info)
            {
                Console.WriteLine("Service lost: " + info.NodeID.ToString() + ", " + info.Name);
            };

            System.Threading.Thread.Sleep(6000);

            var connected_clients = subscription.GetDetectedServiceInfo2();

            foreach (var c in connected_clients)
            {
                Console.WriteLine("Client: " + c.Key.NodeID + ", " + c.Key.ServiceName);
            }

            Console.WriteLine("Waiting for services...");

            Console.ReadLine();

            RobotRaconteurNode.s.Shutdown();

            return;
        }

        if (command == "nowutc")
        {
            Console.WriteLine(RobotRaconteurNode.s.NowUTC);

            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "testlogging")
        {
            var r = new RRLogRecord();
            var node = RobotRaconteurNode.s;
            var nodeid = node.NodeID;
            r.Node = node;
            r.Time = DateTime.UtcNow;
            r.Level = LogLevel.LogLevel_Warning;
            r.Message = "This is a test warning";
            RobotRaconteurNode.s.LogRecord(r);

            RobotRaconteurNode.s.Shutdown();
            return;
        }

        if (command == "testloghandler")
        {
            var user_log_handler = new UserLogRecordHandler(x => Console.WriteLine("csharp handler: " + x.ToString()));
            RobotRaconteurNode.s.SetLogRecordHandler(user_log_handler);
            RobotRaconteurNode.s.SetLogLevel(LogLevel.LogLevel_Debug);

            TcpTransport t = new TcpTransport();
            t.StartServer(2323);

            RobotRaconteurNode.s.RegisterTransport(t);

            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
            RobotRaconteurNode.s.RegisterServiceType(
                new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

            RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
            sup.RegisterServices(t);
            ServiceTestClient c = new ServiceTestClient();
            c.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService",
                          "tcp://localhost:2323/{0}/RobotRaconteurTestService_auth");

            RobotRaconteurNode.s.Shutdown();
            Console.WriteLine("Test completed");

            return;
        }

        if (command == "server2")
        {

            ServerNodeSetup node_setup = new ServerNodeSetup("testprog", 22222, args);
            using (node_setup)
            {
                var t = node_setup.TcpTransport;

                RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
                sup.RegisterServices(t);

                RobotRaconteurTestServiceSupport2 sup2 = new RobotRaconteurTestServiceSupport2();
                sup2.RegisterServices(t);

                RobotRaconteurTestServiceSupport3 sup3 = new RobotRaconteurTestServiceSupport3();
                sup3.RegisterServices();

                Console.WriteLine("Server started, press enter to quit");
                Console.ReadLine();
                RobotRaconteurNode.s.Shutdown();
                Console.WriteLine("Test complete, no error detected");
                return;
            }
        }

        throw new Exception("Unknown command");
    }

    static void print_ServiceInfo2(ServiceInfo2 s)
    {
        Console.WriteLine("Name: " + s.Name);
        Console.WriteLine("RootObjectType: " + s.RootObjectType);
        Console.WriteLine("RootObjectImplements: " + String.Join(", ", s.RootObjectImplements));
        Console.WriteLine("ConnectionURL: " + String.Join(", ", s.ConnectionURL));
        Console.WriteLine("Attributes: " + s.Attributes);
        Console.WriteLine("NodeID: " + s.NodeID.ToString());
        Console.WriteLine("NodeName: " + s.NodeName);
        Console.WriteLine("");
    }

    static void print_NodeInfo2(NodeInfo2 n)
    {
        Console.WriteLine("NodeID: " + n.NodeID.ToString());
        Console.WriteLine("NodeName: " + n.NodeName);
        Console.WriteLine("ConnectionURL: " + String.Join(", ", n.ConnectionURL));
        Console.WriteLine("");
    }

    static bool servicetest_keepgoing = false;
    static ulong servicetest_count = 0;

    static object servicetest_lock = new object();

    static void servicetest7(Pipe<double>.PipeEndpoint p)
    {
        lock (servicetest_lock)
        {
            while (p.Available > 0)
            {

                p.ReceivePacket();
            }
        }
    }

    static void servicetest6(uint pnum, Exception e)
    {}

    static async void servicetest5(Pipe<double>.PipeEndpoint p, Wire<double>.WireConnection w, TimerEvent ev)
    {
        if (ev.stopped)
            return;

        await p.AsyncSendPacket((double)servicetest_count);
        for (double i = 0; i < 100; i++)
        {
            double d = ((double)servicetest_count) * 100 + i;
            w.OutValue = d;
        }
        servicetest_count++;
    }

    /*static void servicetest4(string url1)
    {

        RobotRaconteurNode.s.AsyncConnectService(url1, null, null, null, null, delegate(object o, Exception exp1) {
    servicetest3(url1, o, exp1); });
    }

    static void servicetest3(string url1, object obj, Exception exp)
    {
        if (exp != null)
        {
            Console.WriteLine("Got exception");
            if (servicetest_keepgoing) Environment.Exit(1);
        }

        RobotRaconteurNode.s.AsyncDisconnectService(obj, delegate() { servicetest4(url1); });

    }*/

    static async Task servicetest2(async_testroot o, Task<double> t1)
    {
        /*if (t1 != null)
        {
            Console.WriteLine("Got exception");

        }*/

        while (true)
        {
            await o.async_func3(1, 2);
            if (servicetest_keepgoing)
            {
                Environment.Exit(1);
                return;
            }
        }
    }

    /*static void sevicetest1(object obj, Exception exp)
    {
        if (exp != null)
        {
            Console.WriteLine("Got exception");
            if (servicetest_keepgoing) Environment.Exit(1);
        }

        async_testroot o = (async_testroot)obj;
        o.async_func3(1, 2, delegate(double d, Exception exp1) { servicetest2(o, d, exp1); });


    }*/
}
}

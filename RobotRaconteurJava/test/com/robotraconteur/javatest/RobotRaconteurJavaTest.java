package com.robotraconteur.javatest;
import java.util.*;

import com.robotraconteur.*;

import java.io.*;
import java.util.Random;

import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;
public class RobotRaconteurJavaTest
{

    public static void main(String[] args)
    {

        String command = "loopback";
        if (args.length > 0)
        {
            command = args[0];
        }

        MultiDimArrayTest.testdatapath = "../../out/testdata";

        try
        {

            if (command.equals("loopback"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                TcpTransport c = new TcpTransport();
                c.enableNodeDiscoveryListening();
                c.enableNodeAnnounce();
                c.startServer(2323);
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService2Factory());
                RobotRaconteurNode.s().registerTransport(c);

                RobotRaconteurTestServiceSupport s = new RobotRaconteurTestServiceSupport();
                s.RegisterServices(c);

                /*testroot
                r=(testroot)RobotRaconteurNode.s().connectService("tcp://localhost:2323/{0}/RobotRaconteurTestService");

                System.out.println(r.get_d1());
                System.out.println(r.get_d2().length);
                r.set_d1(3.456);*/

                int count = 1;

                if (args.length >= 2)
                {
                    count = Integer.parseInt(args[1]);
                }

                // MultiDimArrayTest.Test();

                System.out.println("Connect");
                for (int i = 0; i < count; i++)
                {
                    ServiceTestClient stest = new ServiceTestClient();
                    stest.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService",
                                      "tcp://localhost:2323/{0}/RobotRaconteurTestService_auth");
                }
                RobotRaconteurNode.s().shutdown();
                System.out.println("Finished shutdown");
                return;
            }

            if (command.equals("loopback2"))
            {

                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                ServerNodeSetup node_setup = new ServerNodeSetup(
                    "com.robotraconteur.testing.TestService2", 2323,
                    RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT.swigValue() |
                        RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
                            .swigValue());

                try
                {
                    RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService1Factory());
                    RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService2Factory());
                    RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService3Factory());

                    RobotRaconteurTestServiceSupport2 s = new RobotRaconteurTestServiceSupport2();
                    s.RegisterServices(node_setup.getTcpTransport());

                    ServiceTestClient2 stest = new ServiceTestClient2();
                    stest.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService2");

                    RobotRaconteurNode.s().shutdown();
                    System.out.println("Finished shutdown");
                    return;
                }
                finally
                {
                    node_setup.finalize();
                }
            }

            if (command.equals("client"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String url = args[1];
                String url_auth = args[2];

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                HardwareTransport t4 = new HardwareTransport();
                RobotRaconteurNode.s().registerTransport(t4);

                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService2Factory());

                int count = 1;

                if (args.length >= 4)
                {
                    count = Integer.parseInt(args[3]);
                }

                for (int i = 0; i < count; i++)
                {
                    ServiceTestClient stest = new ServiceTestClient();
                    stest.RunFullTest(url, url_auth);
                }
                RobotRaconteurNode.s().shutdown();
                System.out.println("Finished shutdown");
                return;
            }

            if (command.equals("client2"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String url = args[1];

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                HardwareTransport t4 = new HardwareTransport();
                RobotRaconteurNode.s().registerTransport(t4);

                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService2Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService3Factory());

                ServiceTestClient2 stest = new ServiceTestClient2();
                stest.RunFullTest(url);

                RobotRaconteurNode.s().shutdown();
                System.out.println("Finished shutdown");
                return;
            }

            if (command.equals("server"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                int port;
                if (args[1].equals("sharer"))
                {
                    port = -1;
                }
                else
                {
                    port = Integer.parseInt(args[1]);
                }
                String name = args[2];

                LocalTransport t2 = new LocalTransport();
                t2.startServerAsNodeName(name);
                RobotRaconteurNode.s().registerTransport(t2);

                TcpTransport t = new TcpTransport();

                try
                {
                    t.loadTlsNodeCertificate();
                }
                catch (Exception e)
                {
                    System.out.println("warning: Could not load node certificate");
                }

                if (port == -1)
                {
                    t.startServerUsingPortSharer();
                }
                else
                {
                    t.startServer(port);
                }
                t.enableNodeAnnounce();
                RobotRaconteurNode.s().registerTransport(t);

                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService2Factory());
                RobotRaconteurNode.s().registerServiceType(new com__robotraconteur__testing__TestService3Factory());

                RobotRaconteurTestServiceSupport s = new RobotRaconteurTestServiceSupport();
                s.RegisterServices(t);

                RobotRaconteurTestServiceSupport2 s2 = new RobotRaconteurTestServiceSupport2();
                s2.RegisterServices(t);

                System.out.println("Server started, press enter to quit");

                Scanner sc2 = new Scanner(System.in);
                sc2.nextLine();
                sc2.close();
                System.out.println("Done");

                RobotRaconteurNode.s().shutdown();
                System.out.println("Finished shutdown");
                return;
            }

            if (command.equals("findservicebytype"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String type = args[1];
                String[] tschemes = args[2].split(",");

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                Thread.sleep(6000);

                ServiceInfo2[] ret = RobotRaconteurNode.s().findServiceByType(type, tschemes);
                for (ServiceInfo2 s : ret)
                {
                    print_ServiceInfo2(s);
                }

                async_ServiceInfo2 h = new async_ServiceInfo2();
                RobotRaconteurNode.s().asyncFindServiceByType(type, tschemes, h);

                Thread.sleep(10000);

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("findnodebyid"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                NodeID id = new NodeID(args[1]);
                String[] tschemes = args[2].split(",");

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                Thread.sleep(6000);

                NodeInfo2[] ret = RobotRaconteurNode.s().findNodeByID(id, tschemes);
                for (NodeInfo2 s : ret)
                {
                    print_NodeInfo2(s);
                }

                async_NodeInfo2 h = new async_NodeInfo2();
                RobotRaconteurNode.s().asyncFindNodeByID(id, tschemes, h);

                Thread.sleep(10000);

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("findnodebyname"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String name = args[1];
                String[] tschemes = args[2].split(",");

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                Thread.sleep(6000);

                NodeInfo2[] ret = RobotRaconteurNode.s().findNodeByName(name, tschemes);
                for (NodeInfo2 s : ret)
                {
                    print_NodeInfo2(s);
                }

                async_NodeInfo2 h = new async_NodeInfo2();
                RobotRaconteurNode.s().asyncFindNodeByName(name, tschemes, h);

                Thread.sleep(10000);

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("peeridentity"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                if (args.length < 2)
                {
                    System.out.println("Usage for peeridentity: RobotRaconteurTest peeridentity url [nodeid]");
                    return;
                }

                String url1 = args[1];
                TcpTransport c = new TcpTransport();
                if (args.length > 2)
                {
                    String nodeid = args[2];
                    NodeID id = new NodeID(nodeid);

                    RobotRaconteurNode.s().setNodeID(id);
                }

                try
                {
                    c.loadTlsNodeCertificate();
                }
                catch (Exception e)
                {
                    System.out.println("warning: Could not load local node certificate");
                }

                LocalTransport c2 = new LocalTransport();

                HardwareTransport c5 = new HardwareTransport();

                RobotRaconteurNode.s().registerTransport(c);
                RobotRaconteurNode.s().registerTransport(c2);

                RobotRaconteurNode.s().registerTransport(c5);

                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                testroot oo = (testroot)RobotRaconteurNode.s().connectService(url1);
                oo.func3(1.0, 2.3);

                if (c.isTransportConnectionSecure(oo))
                {
                    System.out.println("Connection is secure");

                    if (c.isSecurePeerIdentityVerified(oo))
                    {
                        System.out.println("Peer identity is verified: " + c.getSecurePeerIdentity(oo));
                    }
                    else
                    {
                        System.out.println("Peer identity is not verified");
                    }
                }
                else
                {
                    System.out.println("Connection is not secure");
                }

                RobotRaconteurNode.s().shutdown();

                System.out.println("Test completed, no errors detected");
                return;
            }

            if (command.equals("multidimarraytest"))
            {
                MultiDimArrayTest.Test();
                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("subscribertest"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String type = args[1];

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                ServiceSubscription sub = RobotRaconteurNode.s().subscribeServiceByType(
                    new String[] {"com.robotraconteur.testing.TestService1.testroot"});

                sub.addConnectListener(new subscription_connect_listener());
                sub.addDisconnectListener(new subscription_disconnect_listener());

                WireSubscription<double[]> broadcastwire = sub.<double[]>subscribeWire("broadcastwire");
                broadcastwire.addWireValueChangedListener(new subscription_wire_listener());

                PipeSubscription<double[]> broadcastpipe = sub.<double[]>subscribePipe("broadcastpipe");
                broadcastpipe.addPipePacketReceivedListener(new subscription_pipe_listener());

                Scanner sc2 = new Scanner(System.in);
                sc2.nextLine();
                sc2.close();

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("subscriberfiltertest"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String type = args[1];

                ServiceSubscriptionFilter f = new ServiceSubscriptionFilter();

                if (args.length >= 3)
                {
                    String subcommand = args[2];

                    if (subcommand.equals("nodeid"))
                    {
                        ServiceSubscriptionFilterNode n = new ServiceSubscriptionFilterNode();
                        n.NodeID = new NodeID(args[3]);
                        f.Nodes = new ServiceSubscriptionFilterNode[] {n};
                    }
                    else if (subcommand.equals("nodename"))
                    {
                        ServiceSubscriptionFilterNode n = new ServiceSubscriptionFilterNode();
                        n.NodeName = args[3];
                        f.Nodes = new ServiceSubscriptionFilterNode[] {n};
                    }
                    else if (subcommand.equals("nodeidscheme"))
                    {
                        ServiceSubscriptionFilterNode n = new ServiceSubscriptionFilterNode();
                        n.NodeID = new NodeID(args[3]);
                        f.Nodes = new ServiceSubscriptionFilterNode[] {n};
                        f.TransportSchemes = args[4].split(",");
                    }
                    else if (subcommand.equals("nodeidauth"))
                    {
                        ServiceSubscriptionFilterNode n = new ServiceSubscriptionFilterNode();
                        n.NodeID = new NodeID(args[3]);
                        n.Username = args[4];
                        java.util.Map<String, Object> credentials = new java.util.HashMap<String, Object>();
                        credentials.put("password", args[5]);
                        n.Credentials = credentials;
                        f.Nodes = new ServiceSubscriptionFilterNode[] {n};
                    }
                    else if (subcommand.equals("servicename"))
                    {
                        f.ServiceNames = new String[] {args[3]};
                    }
                    else if (subcommand.equals("predicate"))
                    {
                        f.Predicate = new subscription_predicate();
                    }
                    else
                    {
                        throw new Exception("Invalid subcommand");
                    }
                }

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                ServiceSubscription sub = RobotRaconteurNode.s().subscribeServiceByType(
                    new String[] {"com.robotraconteur.testing.TestService1.testroot"}, f);

                sub.addConnectListener(new subscription_connect_listener());
                sub.addDisconnectListener(new subscription_disconnect_listener());

                Scanner sc2 = new Scanner(System.in);
                sc2.nextLine();
                sc2.close();

                RobotRaconteurNode.s().shutdown();

                return;
            }

            if (command.equals("serviceinfo2subscribertest"))
            {
                RobotRaconteurNode.s().setLogLevelFromEnvVariable();

                String type = args[1];

                TcpTransport t = new TcpTransport();
                t.enableNodeDiscoveryListening();
                RobotRaconteurNode.s().registerTransport(t);

                LocalTransport t2 = new LocalTransport();
                RobotRaconteurNode.s().registerTransport(t2);

                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                ServiceInfo2Subscription sub = RobotRaconteurNode.s().subscribeServiceInfo2(
                    new String[] {"com.robotraconteur.testing.TestService1.testroot"});

                sub.addServiceDetectedListener(new subscription_detected_listener());
                sub.addServiceLostListener(new subscription_lost_listener());

                Thread.sleep(3000);

                Map<ServiceSubscriptionClientID, ServiceInfo2> info = sub.getDetectedServiceInfo2();

                for (ServiceSubscriptionClientID id : info.keySet())
                {
                    System.out.println("Detected services: " + id.NodeID.toString() + ", " + id.ServiceName);
                }

                Scanner sc2 = new Scanner(System.in);
                sc2.nextLine();
                sc2.close();

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("nowutc"))
            {
                System.out.println(RobotRaconteurNode.s().nowUTC());

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("testlogging"))
            {
                RRLogRecord r = new RRLogRecord();
                RobotRaconteurNode node = RobotRaconteurNode.s();
                NodeID nodeid = node.getNodeID();
                r.setNode(node);
                r.setTime(RobotRaconteurNode.s().nowUTC());
                r.setLevel(LogLevel.LogLevel_Warning);
                r.setMessage("This is a test warning");
                RobotRaconteurNode.s().logRecord(r);

                RobotRaconteurNode.s().shutdown();
                return;
            }

            if (command.equals("testloghandler"))
            {
                UserLogRecordHandler user_log_handler = new UserLogRecordHandler(new UserLogRecordHandler_PrintLn());
                RobotRaconteurNode.s().setLogRecordHandler(user_log_handler);
                RobotRaconteurNode.s().setLogLevel(LogLevel.LogLevel_Debug);

                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService1.com__robotraconteur__testing__TestService1Factory());
                RobotRaconteurNode.s().registerServiceType(
                    new com.robotraconteur.testing.TestService2.com__robotraconteur__testing__TestService2Factory());

                TcpTransport t = new TcpTransport();
                t.startServer(2323);

                RobotRaconteurNode.s().registerTransport(t);

                RobotRaconteurTestServiceSupport sup = new RobotRaconteurTestServiceSupport();
                sup.RegisterServices(t);
                ServiceTestClient c = new ServiceTestClient();
                c.RunFullTest("tcp://localhost:2323/{0}/RobotRaconteurTestService",
                              "tcp://localhost:2323/{0}/RobotRaconteurTestService_auth");

                RobotRaconteurNode.s().shutdown();
                System.out.println("Test completed");

                return;
            }

            throw new Exception("Unknown command");
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
            RobotRaconteurNode.s().shutdown();
        }
    }

    static class UserLogRecordHandler_PrintLn implements Action1<RRLogRecord>
    {
        @Override public void action(RRLogRecord rec)
        {
            System.out.println("java handler: " + rec.toString());
        }
    }

    static void print_ServiceInfo2(ServiceInfo2 s)
    {
        System.out.println("Name: " + s.Name);
        System.out.println("RootObjectType: " + s.RootObjectType);
        System.out.println("RootObjectImplements: " + join(", ", s.RootObjectImplements));
        System.out.println("ConnectionURL: " + join(", ", s.ConnectionURL));
        System.out.println("Attributes: " + s.Attributes);
        System.out.println("NodeID: " + s.NodeID.toString());
        System.out.println("NodeName: " + s.NodeName);
        System.out.println("");
    }

    static void print_NodeInfo2(NodeInfo2 n)
    {
        System.out.println("NodeID: " + n.NodeID.toString());
        System.out.println("NodeName: " + n.NodeName);
        System.out.println("ConnectionURL: " + join(", ", n.ConnectionURL));
        System.out.println("");
    }

    static String join(String s, String[] a)
    {
        if (a.length == 0)
            return "";
        String o = a[0];
        for (int i = 1; i < a.length; i++)
        {
            o += (s + a[i]);
        }
        return o;
    }

    protected static class async_ServiceInfo2 implements Action1<ServiceInfo2[]>
    {

        @Override public void action(ServiceInfo2[] p1)
        {
            for (ServiceInfo2 s : p1)
            {
                print_ServiceInfo2(s);
            }
        }
    }

    protected static class async_NodeInfo2 implements Action1<NodeInfo2[]>
    {

        @Override public void action(NodeInfo2[] p1)
        {
            for (NodeInfo2 s : p1)
            {
                print_NodeInfo2(s);
            }
        }
    }

    protected static class subscription_connect_listener
        implements Action3<ServiceSubscription, ServiceSubscriptionClientID, Object>
    {
        @Override public void action(ServiceSubscription subscription, ServiceSubscriptionClientID id, Object client)
        {
            System.out.println("Client connected: " + id.NodeID.toString() + "," + id.ServiceName);
        }
    }

    protected static class subscription_disconnect_listener
        implements Action3<ServiceSubscription, ServiceSubscriptionClientID, Object>
    {
        @Override public void action(ServiceSubscription subscription, ServiceSubscriptionClientID id, Object client)
        {
            System.out.println("Client disconnected: " + id.NodeID.toString() + "," + id.ServiceName);
        }
    }

    protected static class subscription_wire_listener implements Action3<WireSubscription<double[]>, double[], TimeSpec>
    {
        @Override public void action(WireSubscription<double[]> sub, double[] val, TimeSpec time)
        {
            // System.out.println(val[0].toString());
        }
    }

    protected static class subscription_pipe_listener implements Action1<PipeSubscription<double[]>>
    {
        @Override public void action(PipeSubscription<double[]> sub)
        {
            while (sub.available() > 0)
            {
                double[] val = sub.receivePacket();
                System.out.println(Double.toString(val[0]));
            }
        }
    }

    protected static class subscription_predicate implements Func1<ServiceInfo2, Boolean>
    {
        @Override public Boolean func(ServiceInfo2 info)
        {
            System.out.println("Predicate: " + info.NodeName);
            return info.NodeName.equals("testprog");
        }
    }

    protected static class subscription_detected_listener
        implements Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>
    {
        @Override
        public void action(ServiceInfo2Subscription subscription, ServiceSubscriptionClientID id, ServiceInfo2 client)
        {
            System.out.println("Service detected: " + id.NodeID.toString() + "," + id.ServiceName);
        }
    }

    protected static class subscription_lost_listener
        implements Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>
    {
        @Override
        public void action(ServiceInfo2Subscription subscription, ServiceSubscriptionClientID id, ServiceInfo2 client)
        {
            System.out.println("Service lost: " + id.NodeID.toString() + "," + id.ServiceName);
        }
    }
}

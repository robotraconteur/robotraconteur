package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;

public class RobotRaconteurTestServiceSupport2
{
    public testroot3_impl testservice;

    public final void RegisterServices(TcpTransport tcptransport)
    {
        testservice = new testroot3_impl();
        ServerContext c = RobotRaconteurNode.s().registerService(
            "RobotRaconteurTestService2", "com.robotraconteur.testing.TestService3", testservice);
        c.requestObjectLock("RobotRaconteurTestService2.nolock_test", "server");
    }

    public final void UnregisterServices()
    {
        RobotRaconteurNode.s().closeService("RobotRaconteurTestService2");
    }
}
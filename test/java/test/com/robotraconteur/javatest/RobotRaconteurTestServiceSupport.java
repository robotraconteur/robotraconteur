package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

public class RobotRaconteurTestServiceSupport
{
    public RobotRaconteurTest_testroot testservice;
    public RobotRaconteurTest_testroot testservice_auth;

    public final void RegisterServices(TcpTransport tcptransport)
    {
        testservice = new RobotRaconteurTest_testroot(tcptransport);
        testservice_auth = new RobotRaconteurTest_testroot(tcptransport);

        ServerContext c = RobotRaconteurNode.s().registerService(
            "RobotRaconteurTestService", "com.robotraconteur.testing.TestService1", testservice);
        // c.AddServerServiceListener(service_listener);

        String authdata =
            "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride";
        PasswordFileUserAuthenticator p = new PasswordFileUserAuthenticator(authdata);
        java.util.HashMap<String, String> policies = new java.util.HashMap<String, String>();
        policies.put("requirevaliduser", "true");
        policies.put("allowobjectlock", "true");
        ServiceSecurityPolicy s = new ServiceSecurityPolicy(p, policies);

        RobotRaconteurNode.s().registerService("RobotRaconteurTestService_auth",
                                               "com.robotraconteur.testing.TestService1", testservice_auth, s);
    }

    public final void service_listener(ServerContext service, ServerServiceListenerEventType ev, Object parameter)
    {
        System.out.println("Server event: " + ev + " " + parameter.toString());
    }

    public final void UnregisterServices()
    {
        RobotRaconteurNode.s().closeService("RobotRaconteurTestService");
        RobotRaconteurNode.s().closeService("RobotRaconteurTestService_auth");
    }
}
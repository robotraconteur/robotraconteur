import RobotRaconteur as RR

import sys
import os

from .ServiceTest import testroot_impl
from .ServiceTest2 import testroot3_impl
from .ServiceTest3 import asynctestroot_impl


class TestServerNodeConfig:
    def __init__(self, nodename):
        node_env = os.environ.get("ROBOTRACONTEUR_TEST_SERVER_URL", None)
        if node_env is not None:
            self.node_endpoint_url = node_env
            self.node_setup = None
            self.node = None
        else:
            self.node = RR.RobotRaconteurNode()
            self.node.Init()

            server_flags = RR.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT
            server_flags &= ~RR.RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
            self.node_setup = RR.RobotRaconteurNodeSetup(
                nodename, 0, server_flags, RR.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE, node=self.node)

            self.node.RegisterServiceTypesFromFiles(["com.robotraconteur.testing.TestService2", "com.robotraconteur.testing.TestService1",
                                                     "com.robotraconteur.testing.TestService3", "com.robotraconteur.testing.TestService4", "com.robotraconteur.testing.TestService5"])

            t = self.node_setup.tcp_transport

            t2 = testroot_impl(t)
            self.node.RegisterService(
                "RobotRaconteurTestService", "com.robotraconteur.testing.TestService1.testroot", t2)

            t3 = testroot_impl(t)
            authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride"
            p = RR.PasswordFileUserAuthenticator(authdata)
            policies = {"requirevaliduser": "true", "allowobjectlock": "true"}
            s = RR.ServiceSecurityPolicy(p, policies)

            self.node.RegisterService("RobotRaconteurTestService_auth",
                                      "com.robotraconteur.testing.TestService1.testroot", t3, s)

            t4 = testroot3_impl()
            c = self.node.RegisterService(
                "RobotRaconteurTestService2", "com.robotraconteur.testing.TestService3.testroot3", t4)
            c.RequestObjectLock(
                "RobotRaconteurTestService2.nolock_test", "server")

            t3 = asynctestroot_impl()
            c = self.node.RegisterService(
                "RobotRaconteurTestService3", "com.robotraconteur.testing.TestService5.asynctestroot", t3)

    def get_service_url(self, service_name):

        if self.node_setup is None:
            if "?" in service_name:
                return self.node_endpoint_url + "&service=" + service_name
            else:
                return self.node_endpoint_url + "?service=" + service_name

        port = self.node_setup.tcp_transport.GetListenPort()
        return "rr+tcp://localhost:" + str(port) + "?service=" + service_name

    def shutdown(self):
        try:
            if self.node:
                self.node.Shutdown()
                self.node = None
        except:
            pass

    def __enter__(self):
        pass

    def __exit__(self):
        try:
            if self.node:
                self.node.Shutdown()
                self.node = None
        except:
            pass

    def __del__(self):
        try:
            if self.node:
                self.node.Shutdown()
                self.node = None
        except:
            pass

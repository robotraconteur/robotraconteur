import RobotRaconteur as RR
from .ServiceTest import testroot_impl
from .ServiceTest2 import testroot3_impl
from .ServiceTest3 import asynctestroot_impl
import os

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():

    robdef_path = os.path.abspath(os.path.dirname(
        os.path.abspath(__file__)) + "/../../robdef")
    os.environ["ROBOTRACONTEUR_ROBDEF_PATH"] = robdef_path

    node = RR.RobotRaconteurNode()
    node.Init()

    node.RegisterServiceTypesFromFiles(["com.robotraconteur.testing.TestService2", "com.robotraconteur.testing.TestService1",
                                        "com.robotraconteur.testing.TestService3", "com.robotraconteur.testing.TestService4", "com.robotraconteur.testing.TestService5"])

    with RR.ServerNodeSetup("py_testprog", 22223) as node_setup:
        t = node_setup.tcp_transport

        t2 = testroot_impl(t)
        node.RegisterService("RobotRaconteurTestService",
                             "com.robotraconteur.testing.TestService1.testroot", t2)

        t3 = testroot_impl(t)
        authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride"
        p = RR.PasswordFileUserAuthenticator(authdata)
        policies = {"requirevaliduser": "true", "allowobjectlock": "true"}
        s = RR.ServiceSecurityPolicy(p, policies)

        node.RegisterService("RobotRaconteurTestService_auth",
                             "com.robotraconteur.testing.TestService1.testroot", t3, s)

        t4 = testroot3_impl()
        c = node.RegisterService("RobotRaconteurTestService2",
                                 "com.robotraconteur.testing.TestService3.testroot3", t4)
        c.RequestObjectLock("RobotRaconteurTestService2.nolock_test", "server")

        t3 = asynctestroot_impl()
        c = node.RegisterService("RobotRaconteurTestService3",
                                 "com.robotraconteur.testing.TestService5.asynctestroot", t3)

        print("Server ready")
        raw_input("Press enter to quit")


if __name__ == "__main__":
    main()

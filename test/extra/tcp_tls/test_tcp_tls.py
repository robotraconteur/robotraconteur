import RobotRaconteur as RR
import pytest


_tls_test_service_def = """
service experimental.tls_test

object testobj
    function double add_two_numbers(double a, double b)
end object
"""


class _testobj_impl():

    def add_two_numbers(self, a, b):
        return a + b


def test_robotraconteur_tcp_tls():
    node1 = RR.RobotRaconteurNode()
    node1.Init()
    node2 = RR.RobotRaconteurNode()
    node2.Init()
    node3 = RR.RobotRaconteurNode()
    node3.Init()
    node4 = RR.RobotRaconteurNode()
    node4.Init()

    tls_client_flags = RR.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT \
        | RR.RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
    client_override = RR.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE

    node1_setup = RR.SecureServerNodeSetup("testprog_tls1", 0, node=node1)
    node2_setup = RR.ClientNodeSetup(node=node2)
    node3_setup = RR.RobotRaconteurNodeSetup(node_name="testprog_tls2", tcp_port=0, flags=tls_client_flags,
                                             allowed_overrides=client_override, node=node3)
    node4_setup = RR.RobotRaconteurNodeSetup(node_name="testprog_tls3", tcp_port=0, flags=tls_client_flags,
                                             allowed_overrides=client_override, node=node4)

    node1.RegisterServiceType(_tls_test_service_def)

    obj = _testobj_impl()
    obj2 = _testobj_impl()
    ctx = node1.RegisterService(
        "test_service", "experimental.tls_test.testobj", obj)

    authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock " + str(node3.NodeID) + "\n" \
        + "testuser2 841c4221c2e7e0cefbc0392a35222512 objectlock " + str(node4.NodeID) + "\n" \
        + "testsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride " + \
        str(node3.NodeID) + "\n"
    p = RR.PasswordFileUserAuthenticator(authdata, True)
    policies = {"requirevaliduser": "true", "allowobjectlock": "true"}
    s = RR.ServiceSecurityPolicy(p, policies)
    ctx_secure = node1.RegisterService(
        "test_service_secure", "experimental.tls_test.testobj", obj2, s)

    server_tcp_port = node1_setup.tcp_transport.GetListenPort()

    url = f"rrs+tcp://localhost:{server_tcp_port}?service=test_service"
    url_unsecure = f"rr+tcp://localhost:{server_tcp_port}?service=test_service"
    url_ws = f"rrs+ws://localhost:{server_tcp_port}?service=test_service"
    url_unsecure_ws = f"rr+ws://localhost:{server_tcp_port}?service=test_service"
    url_secure = f"rrs+tcp://localhost:{server_tcp_port}?service=test_service_secure"

    with node1_setup, node2_setup, node3_setup, node4_setup:
        c1 = node2.ConnectService(url)
        assert c1.add_two_numbers(5, 6) == 11
        node2.DisconnectService(c1)

        # Make sure that unencrypted connections are dropped
        with pytest.raises(RR.ConnectionException):
            c2 = node2.ConnectService(url_unsecure)

        c1 = node2.ConnectService(url_ws)
        assert c1.add_two_numbers(5, 6) == 11
        node2.DisconnectService(c1)

        # Make sure that unencrypted connections are dropped
        with pytest.raises(RR.ConnectionException):
            c2 = node2.ConnectService(url_unsecure_ws)
        user1 = "testuser1"
        user2 = "testuser2"
        cred1 = {"password": RR.VarValue("testpass1", "string")}
        cred2 = {"password": RR.VarValue("testpass2", "string")}

        with pytest.raises(RR.AuthenticationException):
            node2.ConnectService(url_secure, user1, cred1)

        c3 = node3.ConnectService(url_secure, user1, cred1)
        assert c3.add_two_numbers(5, 7) == 12
        node3.DisconnectService(c3)

        with pytest.raises(RR.AuthenticationException):
            node4.ConnectService(url_secure, user1, cred1)

        c4 = node4.ConnectService(url_secure, user2, cred2)
        assert c4.add_two_numbers(9, 7) == 16
        node4.DisconnectService(c4)

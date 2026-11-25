import RobotRaconteur as RR
import pytest

_shadow_test_service_def = """
service experimental.user_auth_shadow_test

object testobj
    property int32 p1
end
"""


class _testobj_impl:
    def __init__(self):
        self.p1 = 5


class _rejecting_authenticator:
    def __init__(self):
        self.calls = 0

    def AuthenticateUser(self, username, credentials, context):
        self.calls += 1
        raise RR.AuthenticationException("Invalid username or token")


def test_python_user_authenticator_is_called():
    server_node = RR.RobotRaconteurNode()
    server_node.Init()
    server_node.SetNodeName("user_auth_shadow_test_server")
    t1 = RR.IntraTransport(server_node)
    server_node.RegisterTransport(t1)
    t1.StartServer()
    server_node.RegisterServiceType(_shadow_test_service_def)
    authenticator = _rejecting_authenticator()
    # A Python object must be accepted as the authenticator (wrapped by the ServiceSecurityPolicy shadow)
    policy = RR.ServiceSecurityPolicy(
        authenticator, {"requirevaliduser": "true", "allowobjectlock": "true"})
    server_node.RegisterService(
        "test_service", "experimental.user_auth_shadow_test.testobj", _testobj_impl(), policy)

    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    t2 = RR.IntraTransport(client_node)
    client_node.RegisterTransport(t2)
    t2.StartClient()

    try:
        url = "rr+intra:///?nodename=user_auth_shadow_test_server&service=test_service"
        with pytest.raises(RR.AuthenticationException):
            client_node.ConnectService(
                url, "someuser", {"token": RR.VarValue("sometoken", "string")})
        assert authenticator.calls == 1
    finally:
        client_node.Shutdown()
        server_node.Shutdown()

import RobotRaconteur as RR

_user_auth_test_service_def = """
service experimental.user_auth_test

object testobj
    property int32 p1
end
"""


class _testobj_impl:
    def __init__(self):
        self.p1 = 5


class _custom_authenticator:
    def __init__(self):
        self.calls = 0

    def AuthenticateUser(self, username, credentials, context):
        self.calls += 1
        if username != "customuser" or credentials["token"].data != "customtoken":
            raise RR.AuthenticationException("Invalid username or token")
        return RR.AuthenticatedUser(username, ["objectlock"], [], context)


def test_custom_user_authenticator():
    server_node = RR.RobotRaconteurNode()
    server_node.Init()
    server_node.SetNodeName("user_auth_test_server")
    t1 = RR.IntraTransport(server_node)
    server_node.RegisterTransport(t1)
    t1.StartServer()
    server_node.RegisterServiceType(_user_auth_test_service_def)
    authenticator = _custom_authenticator()
    policy = RR.ServiceSecurityPolicy(
        authenticator, {"requirevaliduser": "true", "allowobjectlock": "true"})
    server_node.RegisterService(
        "test_service", "experimental.user_auth_test.testobj", _testobj_impl(), policy)

    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    t2 = RR.IntraTransport(client_node)
    client_node.RegisterTransport(t2)
    t2.StartClient()

    try:
        url = "rr+intra:///?nodename=user_auth_test_server&service=test_service"
        c = client_node.ConnectService(
            url, "customuser", {"token": RR.VarValue("customtoken", "string")})
        assert c.p1 == 5
        client_node.DisconnectService(c)
        assert authenticator.calls == 1
    finally:
        client_node.Shutdown()
        server_node.Shutdown()

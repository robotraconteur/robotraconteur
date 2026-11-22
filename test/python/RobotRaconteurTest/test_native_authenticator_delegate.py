import RobotRaconteur as RR
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import WrappedUserAuthenticatorDirectorPython
import pytest

_robdef = """
service experimental.native_auth_delegate

object testobj
    property double d1
end
"""


class _testobj_impl(object):
    def __init__(self):
        self.d1 = 1.0


class _delegating_authenticator(object):
    # A Python authenticator that delegates to a native PasswordFileUserAuthenticator
    def __init__(self, native):
        self.native = native

    def AuthenticateUser(self, username, credentials, context):
        return self.native.AuthenticateUser(username, credentials, context)


def _run(allowed_nodeid, server_name):
    server = RR.RobotRaconteurNode()
    server.Init()
    server.SetNodeName(server_name)
    t1 = RR.IntraTransport(server)
    server.RegisterTransport(t1)
    t1.StartServer()
    server.RegisterServiceType(_robdef)

    client = RR.RobotRaconteurNode()
    client.Init()
    t2 = RR.IntraTransport(client)
    client.RegisterTransport(t2)
    try:
        if allowed_nodeid is None:
            allowed_nodeid = client.NodeID
        authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock " + allowed_nodeid.ToString("D")
        native = RR.PasswordFileUserAuthenticator(authdata, True)
        policies = {"requirevaliduser": "true"}
        # Wrap the Python authenticator the same way the ServiceSecurityPolicy shadow constructor does
        d = WrappedUserAuthenticatorDirectorPython(_delegating_authenticator(native))
        auth = RRP.WrappedUserAuthenticator()
        auth.SetRRDirector(d, 0)
        d.__disown__()
        s = RR.ServiceSecurityPolicy(auth, policies)
        server.RegisterService("service1", "experimental.native_auth_delegate.testobj", _testobj_impl(), s)

        cred1 = {"password": RR.VarValue("testpass1", "string")}
        c = client.ConnectService("rr+intra:///?nodename=" + server_name + "&service=service1", "testuser1", cred1)
        try:
            assert c.d1 == 1.0
        finally:
            client.DisconnectService(c)
    finally:
        client.Shutdown()
        server.Shutdown()


def test_native_authenticator_delegate_verified_client():
    # The client NodeID is in the allowed list: authentication succeeds
    _run(None, "native_auth_delegate_server1")


def test_native_authenticator_delegate_rejects_wrong_nodeid():
    # require_verified_client=True and the client NodeID is not allowed. A Python authenticator
    # that delegates to the native authenticator must not skip the client verification.
    with pytest.raises(RR.AuthenticationException):
        _run(RR.NodeID("7f1bb15f-6a1b-4a4e-8fbe-0b3a37b62c55"), "native_auth_delegate_server2")

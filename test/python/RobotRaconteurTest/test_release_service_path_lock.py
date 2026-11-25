import RobotRaconteur as RR
import pytest

# Regression test: ServerContext::ReleaseServicePath1 removed a released object's lock from active_object_locks by
# username instead of by the root service path the map is keyed by. The stale entry made a later lock on the same
# path untracked (map insert does not replace), so that lock was not released when its client disconnected and the
# object stayed locked for everyone else.

_robdef = """
service experimental.lock_release_test

object root
    objref sub o2
    function void replace_o2()
end

object sub
    property double d1
end
"""

_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

_authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\n" \
    "testuser2 841c4221c2e7e0cefbc0392a35222512 objectlock"


class _sub_impl(object):
    def __init__(self):
        self.d1 = 0.0


class _root_impl(object):
    def __init__(self):
        self._o2 = _sub_impl()

    def get_o2(self):
        return self._o2, "experimental.lock_release_test.sub"

    def replace_o2(self):
        RR.ServerContext.GetCurrentServerContext().ReleaseServicePath(
            RR.ServerContext.GetCurrentServicePath() + ".o2")
        self._o2 = _sub_impl()


def _connect(node, url, user, password):
    return node.ConnectService(url, user, {"password": RR.VarValue(password, "string")})


def test_lock_released_after_service_path_release():
    server_id = RR.NodeID("3f1c2b7a-9d64-4e0b-8a51-6c2f4d9e7b13")
    server = RR.RobotRaconteurNode()
    server.SetNodeID(server_id)
    server.Init()
    server.RegisterServiceType(_robdef)
    policy = RR.ServiceSecurityPolicy(RR.PasswordFileUserAuthenticator(_authdata),
                                      {"requirevaliduser": "true", "allowobjectlock": "true"})
    server_setup = RR.RobotRaconteurNodeSetup("lock_release_server", 0, flags=_server_flags, node=server)
    server.RegisterService("lock_release", "experimental.lock_release_test.root", _root_impl(), policy)

    client_a = RR.RobotRaconteurNode()
    client_a.Init()
    client_a_setup = RR.RobotRaconteurNodeSetup("", 0, flags=_client_flags, node=client_a)
    client_b = RR.RobotRaconteurNode()
    client_b.Init()
    client_b_setup = RR.RobotRaconteurNodeSetup("", 0, flags=_client_flags, node=client_b)

    url = "rr+intra:///?nodeid=" + server_id.ToString("B") + "&service=lock_release"

    with server_setup, client_a_setup, client_b_setup:
        r_a = _connect(client_a, url, "testuser1", "testpass1")
        r_b = _connect(client_b, url, "testuser2", "testpass2")

        # Client A session-locks o2; the service then replaces o2, releasing the locked object's service path
        client_a.RequestObjectLock(r_a.get_o2(), RR.RobotRaconteurObjectLockFlags_CLIENT_LOCK)
        r_a.replace_o2()

        # Client A locks the new o2; client B is locked out
        o2_a = r_a.get_o2()
        client_a.RequestObjectLock(o2_a, RR.RobotRaconteurObjectLockFlags_CLIENT_LOCK)
        o2_a.d1 = 1.0
        o2_b = r_b.get_o2()
        with pytest.raises(RR.ObjectLockedException):
            o2_b.d1 = 2.0

        # Closing client A's connection must release its lock
        client_a.DisconnectService(r_a)
        o2_b = r_b.get_o2()
        o2_b.d1 = 3.0
        assert o2_b.d1 == 3.0

        client_b.DisconnectService(r_b)

    client_a.Shutdown()
    client_b.Shutdown()
    server.Shutdown()

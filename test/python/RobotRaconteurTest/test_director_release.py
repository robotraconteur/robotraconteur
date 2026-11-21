import RobotRaconteur as RR
import gc
import time
import threading
import weakref

_director_release_test_service_def = """
service experimental.director_release_test

object testobj
    pipe double testpipe
    wire double testwire
end
"""

_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


class _testobj_impl:
    def __init__(self):
        self.pipe_ep = None
        self.wire_conn = None
        self.connected = threading.Event()

    def RRServiceObjectInit(self, ctx, service_path):
        self.testpipe.PipeConnectCallback = self._pipe_connected
        self.testwire.WireConnectCallback = self._wire_connected

    def _pipe_connected(self, ep):
        self.pipe_ep = ep
        self.connected.set()

    def _wire_connected(self, conn):
        self.wire_conn = conn
        self.connected.set()


def _wait_collected(ref):
    # The director is released on a Robot Raconteur thread after the closed callback returns
    for _ in range(100):
        gc.collect()
        if ref() is None:
            return True
        time.sleep(0.01)
    return False


def _run_closed_by_peer_test(connect, closed_callback_name, close_server_side):
    obj = _testobj_impl()
    server_node = RR.RobotRaconteurNode()
    server_node.SetNodeID(RR.NodeID("8a3e2c55-6b1d-4f1e-9a51-6b9f3f0e7d21"))
    server_node.Init()
    server_node.RegisterServiceType(_director_release_test_service_def)
    server_node.RegisterService(
        "test_service", "experimental.director_release_test.testobj", obj)
    server_setup = RR.RobotRaconteurNodeSetup(
        "director_release_server", 0, flags=_server_flags, node=server_node)

    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    client_setup = RR.RobotRaconteurNodeSetup(
        "", 0, flags=_client_flags, node=client_node)

    with client_setup, server_setup:
        c = client_node.ConnectService(
            "rr+intra:///?nodename=director_release_server&service=test_service")

        closed = threading.Event()
        ep = connect(c)
        setattr(ep, closed_callback_name, lambda _: closed.set())
        assert obj.connected.wait(5)

        close_server_side(obj)
        assert closed.wait(5)

        ref = weakref.ref(ep)
        del ep
        assert _wait_collected(ref)

        client_node.DisconnectService(c)


def test_pipe_endpoint_released_after_peer_close():
    _run_closed_by_peer_test(lambda c: c.testpipe.Connect(-1), "PipeEndpointClosedCallback",
                             lambda obj: obj.pipe_ep.Close())


def test_wire_connection_released_after_peer_close():
    _run_closed_by_peer_test(lambda c: c.testwire.Connect(), "WireConnectionClosedCallback",
                             lambda obj: obj.wire_conn.Close())

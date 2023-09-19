import RobotRaconteur as RR
import pytest
import time
import threading

_pipe_sub_test_service_def = """
service experimental.pipe_sub_test

object testobj
    pipe double testpipe1 [readonly]
    pipe double testpipe2
    objref testobj2 subobj
end

object testobj2
    pipe double testpipe3 [readonly]
end
"""


class _testobj_impl:
    def __init__(self):
        self.subobj = _testobj2_impl()
        self.recv_packets = []

    def get_subobj(self):
        return self.subobj, "experimental.pipe_sub_test.testobj2"

    def RRServiceObjectInit(self, ctx, service_path):
        self.testpipe2.PipeConnectCallback = self._pipe_ep_connected

    def _pipe_ep_connected(self, pipe_ep):
        pipe_ep.PacketReceivedEvent += self._pipe_packet_received

    def _pipe_packet_received(self, pipe_ep):
        while pipe_ep.Available > 0:
            self.recv_packets.append(pipe_ep.ReceivePacket())


class _testobj2_impl:
    def __init__(self):
        pass


intra_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

intra_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


class _testservice_impl():

    def __init__(self, nodename, nodeid):
        self.obj = _testobj_impl()
        self._node = RR.RobotRaconteurNode()
        self._node.SetNodeID(nodeid)
        self._node.Init()

        self._node.RegisterServiceType(_pipe_sub_test_service_def)
        self._node.RegisterService(
            "test_service", "experimental.pipe_sub_test.testobj", self.obj)

        self._node_setup = RR.RobotRaconteurNodeSetup(
            nodename, 0, flags=intra_server_flags, node=self._node)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._node_setup.ReleaseNode()
        self._node.Shutdown()
        self._node_setup = None
        self._node = None


def test_pipe_subscription():

    test_servers = {
        "server1": RR.NodeID("0d694574-1ad8-4b9e-9aea-e881524fb451"),
        "server2": RR.NodeID("e23ac123-4357-467e-b44b-4c9eb4ff7916"),
        "server3": RR.NodeID("cb71939a-6c6c-43cc-b6be-070a76acec74")
    }

    client_node = RR.RobotRaconteurNode()
    client_node.Init()

    client_node_setup = RR.RobotRaconteurNodeSetup(
        "", 0, flags=intra_client_flags, node=client_node)

    server1 = _testservice_impl("server1", test_servers["server1"])

    with client_node_setup, server1:

        packet_recv_count = [0]

        def packet_recv(ep):
            packet_recv_count[0] += 1

        sub = client_node.SubscribeServiceByType(
            "experimental.pipe_sub_test.testobj")
        pipe_sub = sub.SubscribePipe("testpipe1")
        pipe_sub.PipePacketReceived += packet_recv
        sub.GetDefaultClientWait(5)

        assert pipe_sub.ActivePipeEndpointCount > 0

        with pytest.raises(RR.InvalidOperationException):
            pipe_sub.ReceivePacket()
        assert not pipe_sub.TryReceivePacket()[0]

        server1.obj.testpipe1.SendPacket(1.0)
        server1.obj.testpipe1.SendPacket(2.0)
        time.sleep(0.05)
        res, packet = pipe_sub.TryReceivePacket()
        assert res and packet == 1.0
        assert pipe_sub.ReceivePacket() == 2.0

        def delay_send():
            time.sleep(0.15)
            server1.obj.testpipe1.SendPacket(3.0)
            server1.obj.testpipe1.SendPacket(4.0)

        t = threading.Thread(target=delay_send)
        t.start()

        res, val = pipe_sub.TryReceivePacketWait(1)
        assert res and val == 3.0
        time.sleep(0.005)
        assert pipe_sub.Available == 1
        res, val = pipe_sub.TryReceivePacketWait(1, True)
        assert res and val == 4.0
        assert pipe_sub.Available == 1
        res, val = pipe_sub.TryReceivePacketWait(1)
        assert res and val == 4.0

        pipe_sub2 = sub.SubscribePipe("testpipe2")
        time.sleep(0.1)
        assert pipe_sub2.ActivePipeEndpointCount > 0
        pipe_sub2.AsyncSendPacketAll(5.0)
        pipe_sub2.AsyncSendPacketAll(6.0)

        time.sleep(0.05)

        assert len(server1.obj.recv_packets) == 2

        pipe_sub3 = sub.SubscribePipe("testpipe3", "*.subobj")
        time.sleep(0.1)
        assert pipe_sub3.ActivePipeEndpointCount > 0
        server1.obj.subobj.testpipe3.SendPacket(7.0)
        server1.obj.subobj.testpipe3.SendPacket(8.0)
        res, value = pipe_sub3.TryReceivePacketWait(1)
        assert res and value == 7.0
        res, value = pipe_sub3.TryReceivePacketWait(1)
        assert res and value == 8.0

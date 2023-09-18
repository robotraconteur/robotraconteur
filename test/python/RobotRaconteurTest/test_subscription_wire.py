import RobotRaconteur as RR
import pytest
import time
import threading

_wire_sub_test_service_def = """
service experimental.wire_sub_test

object testobj
    wire double testwire1 [readonly]
    wire double testwire2 [writeonly]
    objref testobj2 subobj
end

object testobj2
    wire double testwire3 [readonly]
end
"""


class _testobj_impl:
    def __init__(self):
        self.subobj = _testobj2_impl()

    def get_subobj(self):
        return self.subobj, "experimental.wire_sub_test.testobj2"


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

        self._node.RegisterServiceType(_wire_sub_test_service_def)
        self._node.RegisterService(
            "test_service", "experimental.wire_sub_test.testobj", self.obj)

        self._node_setup = RR.RobotRaconteurNodeSetup(
            nodename, 0, flags=intra_server_flags, node=self._node)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._node_setup.ReleaseNode()
        self._node.Shutdown()
        self._node_setup = None
        self._node = None


def test_wire_subscription():

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

        value_changed_count = [0]

        def value_changed(wire, value, time):
            value_changed_count[0] += 1

        sub = client_node.SubscribeServiceByType(
            "experimental.wire_sub_test.testobj")
        wire_sub = sub.SubscribeWire("testwire1")
        wire_sub.WireValueChanged += value_changed
        sub.GetDefaultClientWait(5)

        assert wire_sub.ActiveWireConnectionCount > 0

        with pytest.raises(RR.ValueNotSetException):
            _ = wire_sub.InValue
        assert not wire_sub.TryGetInValue()[0]

        server1.obj.testwire1.OutValue = 5.0
        time.sleep(0.001)
        server1.obj.testwire1.OutValue = 5.0

        wire_sub.WaitInValueValid(1)

        assert wire_sub.InValue == 5.0
        assert wire_sub.TryGetInValue()[1] == 5.0

        assert value_changed_count[0] > 0

        wire_sub.InValueLifespan = 0.1
        assert wire_sub.InValueLifespan == 0.1
        time.sleep(0.2)
        assert not wire_sub.TryGetInValue()[0]

        wire_sub.InValueLifespan = 10
        server1.obj.testwire1.OutValue = 6.0
        time.sleep(0.05)
        assert wire_sub.InValue == 6.0

        wire_sub2 = sub.SubscribeWire("testwire3", "*.subobj")

        assert wire_sub2.ActiveWireConnectionCount > 0

        time.sleep(0.2)

        server1.obj.subobj.testwire3.OutValue = 12.345

        wire_sub2.WaitInValueValid(1)
        assert wire_sub2.InValue == 12.345

        wire_sub2.Close()

        wire_sub3 = sub.SubscribeWire("testwire2")
        time.sleep(0.05)
        assert wire_sub3.ActiveWireConnectionCount > 0
        wire_sub3.SetOutValueAll(17.0)

        time.sleep(0.05)
        assert server1.obj.testwire2.InValue[0] == 17.0
        res, val, _, _ = server1.obj.testwire2.TryGetInValue()
        assert res and val == 17.0
        server1.obj.testwire2.InValueLifespan = 0.1
        time.sleep(0.2)
        assert not server1.obj.testwire2.TryGetInValue()[0]

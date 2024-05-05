import RobotRaconteur as RR
import pytest
import time
import threading
import traceback

_sub_test_service_def = """
service experimental.subobject_sub_test

object testobj
    objref testobj2 subobj2
    function string getf_service_path()
end

object testobj2
    objref testobj3 subobj3_1
    objref testobj3[] subobj3_2
    objref testobj3{string} subobj3_3
    function string getf_service_path()
end

object testobj3
    function double add_two_numbers(double a, double b)
    function string getf_service_path()
end
"""


class _testobj3_impl():

    def __init__(self, index=None):
        self._service_path = None
        self._index = index

    def RRServiceObjectInit(self, ctx, service_path):
        self._service_path = service_path

    def add_two_numbers(self, a, b):
        return a + b

    def getf_service_path(self):
        return self._service_path


class _testobj2_impl():

    def __init__(self):
        self._service_path = None

    def RRServiceObjectInit(self, ctx, service_path):
        self._service_path = service_path

    def getf_service_path(self):
        return self._service_path

    def get_subobj3_1(self):
        return _testobj3_impl(), "experimental.subobject_sub_test.testobj3"

    def get_subobj3_2(self, ind):
        return _testobj3_impl(ind), "experimental.subobject_sub_test.testobj3"

    def get_subobj3_3(self, ind):
        return _testobj3_impl(ind), "experimental.subobject_sub_test.testobj3"


class _testobj_impl():

    def __init__(self):
        self._service_path = None

    def RRServiceObjectInit(self, ctx, service_path):
        self._service_path = service_path

    def getf_service_path(self):
        return self._service_path

    def get_subobj2(self):
        return _testobj2_impl(), "experimental.subobject_sub_test.testobj2"


class _testservice_impl():

    def __init__(self, nodename, nodeid):
        self._obj = _testobj_impl()
        self._node = RR.RobotRaconteurNode()
        self._node.SetNodeID(nodeid)
        self._node.Init()

        self._node.RegisterServiceType(_sub_test_service_def)
        self._node.RegisterService(
            "test_service", "experimental.subobject_sub_test.testobj", self._obj)

        self._node_setup = RR.RobotRaconteurNodeSetup(
            nodename, 0, flags=intra_server_flags, node=self._node)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._node_setup.ReleaseNode()
        self._node.Shutdown()
        self._node_setup = None
        self._node = None


intra_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

intra_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


def test_subscription_subobject():

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

        sub = client_node.SubscribeServiceByType(
            "experimental.subobject_sub_test.testobj")
        c = sub.GetDefaultClientWait(5)

        assert c.getf_service_path() == "test_service"

        sub2 = sub.SubscribeSubObject("*.subobj2")
        c2 = sub2.GetDefaultClient()
        assert c2.getf_service_path() == "test_service.subobj2"

        c3 = sub2.GetDefaultClientWait(1)
        assert c3.getf_service_path() == "test_service.subobj2"

        res, c4 = sub2.TryGetDefaultClient()
        assert res and c4.getf_service_path() == "test_service.subobj2"

        res, c5 = sub2.TryGetDefaultClientWait(1)
        assert res and c5.getf_service_path() == "test_service.subobj2"

        evt = threading.Event()

        def handler(c, exp):
            try:
                if exp is not None:
                    raise exp
                assert c.getf_service_path() == "test_service.subobj2"
                print("Got async callback")
                evt.set()
            except:
                traceback.print_exc()
                raise

        sub2.AsyncGetDefaultClient(handler, 1)
        assert evt.wait(1)

        sub3 = sub.SubscribeSubObject("*.subobj2.subobj3_1")
        c6 = sub3.GetDefaultClient()
        assert c6.getf_service_path() == "test_service.subobj2.subobj3_1"

        sub4 = sub.SubscribeSubObject("*.subobj2.subobj3_2[123]")
        c7 = sub4.GetDefaultClient()
        assert c7.getf_service_path() == "test_service.subobj2.subobj3_2[123]"

        sub5 = sub.SubscribeSubObject("*.subobj2.subobj3_3[someobj]")
        c8 = sub5.GetDefaultClient()
        assert c8.getf_service_path() == "test_service.subobj2.subobj3_3[someobj]"

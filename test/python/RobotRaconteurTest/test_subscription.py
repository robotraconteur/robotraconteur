import RobotRaconteur as RR
import pytest
import time
import threading

_sub_test_service_def = """
service experimental.sub_test

object testobj
    function double add_two_numbers(double a, double b)
end
"""

intra_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

intra_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


class _testobj_impl():

    def add_two_numbers(self, a, b):
        return a + b


class _testservice_impl():

    def __init__(self, nodename, nodeid):
        self._obj = _testobj_impl()
        self._node = RR.RobotRaconteurNode()
        self._node.SetNodeID(nodeid)
        self._node.Init()

        self._node.RegisterServiceType(_sub_test_service_def)
        self._node.RegisterService(
            "test_service", "experimental.sub_test.testobj", self._obj)

        self._node_setup = RR.RobotRaconteurNodeSetup(
            nodename, 0, flags=intra_server_flags, node=self._node)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._node_setup.ReleaseNode()
        self._node.Shutdown()
        self._node_setup = None
        self._node = None


def test_subscribe_by_type():

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
            "experimental.sub_test.testobj")
        c = sub.GetDefaultClientWait(5)
        assert c.add_two_numbers(1, 2) == 3
        c2 = sub.GetDefaultClient()
        assert c2.add_two_numbers(3, 2) == 5
        res, c3 = sub.TryGetDefaultClient()
        assert res
        assert c3.add_two_numbers(3, 2) == 5
        res, c4 = sub.TryGetDefaultClientWait(1)
        assert res
        assert c4.add_two_numbers(3, 7) == 10

        c5 = sub.GetConnectedClients()
        assert len(c5) >= 1
        c6 = c5[RR.ServiceSubscriptionClientID(
            test_servers["server1"], "test_service")]
        assert c6.add_two_numbers(3, 7) == 10

        async_client_called = [False]
        async_client_called_evt = threading.Event()

        def async_default_client_handler(client, err):
            assert err is None
            assert client.add_two_numbers(3, 9) == 12
            async_client_called[0] = True
            async_client_called_evt.set()

        sub.AsyncGetDefaultClient(async_default_client_handler, 5)
        async_client_called_evt.wait(5)
        assert async_client_called[0]

        connect_called = [False]
        disconnect_called = [False]

        connect_called_evt = threading.Event()
        disconnect_called_evt = threading.Event()

        def connect_handler(sub, client_id, client):
            assert client_id.NodeID == test_servers["server2"]
            assert client_id.ServiceName == "test_service"
            connect_called[0] = True
            connect_called_evt.set()

        def disconnect_handler(sub, client_id, client):
            assert client_id.NodeID == test_servers["server2"]
            assert client_id.ServiceName == "test_service"
            disconnect_called[0] = True
            disconnect_called_evt.set()

        sub.ClientConnected += connect_handler
        sub.ClientDisconnected += disconnect_handler

        server2 = _testservice_impl("server2", test_servers["server2"])
        with server2:
            connect_called_evt.wait(5)
            assert connect_called[0]

        disconnect_called_evt.wait(5)
        assert disconnect_called[0]

        sub.ClientConnected -= connect_handler
        sub.ClientDisconnected -= disconnect_handler


def test_subscribe_serviceinfo2():

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
        sub = client_node.SubscribeServiceInfo2(
            "experimental.sub_test.testobj")

        wait_count = 0
        while True:
            time.sleep(0.1)
            if len(sub.GetDetectedServiceInfo2()) > 0:
                break
            wait_count += 1
            if wait_count > 50:
                raise Exception("Timeout waiting for service info")

        detected_nodes = sub.GetDetectedServiceInfo2()
        assert len(detected_nodes) >= 1
        service_info = detected_nodes[RR.ServiceSubscriptionClientID(
            test_servers["server1"], "test_service")]
        assert service_info.NodeID == test_servers["server1"]
        assert service_info.Name == "test_service"
        assert service_info.RootObjectType == "experimental.sub_test.testobj"
        assert service_info.Name == "test_service"

        connect_called = [False]
        # disconnect_called = [False]
        connect_called_evt = threading.Event()
        # disconnect_called_evt = threading.Event()

        def connect_handler(sub, client_id, service_info2):
            assert client_id.NodeID == test_servers["server2"]
            assert client_id.ServiceName == "test_service"
            connect_called[0] = True
            connect_called_evt.set()

        # disconnect called takes up to 1 minute

        # def disconnect_handler(sub, client_id, client):
        #     assert client_id.NodeID == test_servers["server2"]
        #     assert client_id.ServiceName == "test_service"
        #     disconnect_called[0] = True
        #     disconnect_called_evt.set()

        sub.ServiceDetected += connect_handler
        # sub.ClientDisconnected += disconnect_handler

        server2 = _testservice_impl("server2", test_servers["server2"])
        with server2:
            connect_called_evt.wait(5)
            assert connect_called[0]

        # disconnect_called_evt.wait(5)
        # assert disconnect_called[0]

        sub.ServiceDetected -= connect_handler
        # sub.ClientDisconnected -= disconnect_handler


def test_subscribe_by_url():

    test_servers = {
        "server1": RR.NodeID("0d694574-1ad8-4b9e-9aea-e881524fb451"),
        "server2": RR.NodeID("e23ac123-4357-467e-b44b-4c9eb4ff7916"),
        "server3": RR.NodeID("cb71939a-6c6c-43cc-b6be-070a76acec74")
    }

    client_node = RR.RobotRaconteurNode()
    client_node.Init()

    client_node_setup = RR.RobotRaconteurNodeSetup(
        "", 0, flags=intra_client_flags, node=client_node)

    with client_node_setup:

        sub = client_node.SubscribeService(
            "rr+intra:///?nodename=server2&service=test_service")

        connect_called = [False]
        disconnect_called = [False]

        connect_called_evt = threading.Event()
        disconnect_called_evt = threading.Event()

        def connect_handler(sub, client_id, client):
            assert client_id.NodeID == test_servers["server2"]
            assert client_id.ServiceName == "test_service"
            connect_called[0] = True
            connect_called_evt.set()

        def disconnect_handler(sub, client_id, client):
            assert client_id.NodeID == test_servers["server2"]
            assert client_id.ServiceName == "test_service"
            disconnect_called[0] = True
            disconnect_called_evt.set()

        sub.ClientConnected += connect_handler
        sub.ClientDisconnected += disconnect_handler

        server2 = _testservice_impl("server2", test_servers["server2"])
        with server2:
            connect_called_evt.wait(5)
            assert connect_called[0]

            c = sub.GetDefaultClient()
            assert c.add_two_numbers(3, 2) == 5

        disconnect_called_evt.wait(5)
        assert disconnect_called[0]

        sub.ClientConnected -= connect_handler
        sub.ClientDisconnected -= disconnect_handler


def test_subscribe_by_url_bad_url():

    test_servers = {
        "server1": RR.NodeID("0d694574-1ad8-4b9e-9aea-e881524fb451"),
        "server2": RR.NodeID("e23ac123-4357-467e-b44b-4c9eb4ff7916"),
        "server3": RR.NodeID("cb71939a-6c6c-43cc-b6be-070a76acec74")
    }

    client_node = RR.RobotRaconteurNode()
    client_node.Init()

    client_node_setup = RR.RobotRaconteurNodeSetup(
        "", 0, flags=intra_client_flags, node=client_node)

    with client_node_setup:

        connect_err_called = [False]
        connect_err_called_evt = threading.Event()

        def connect_err_handler(sub, id, url, err):
            assert err is not None
            assert "RobotRaconteur.ConnectionError" in str(err)
            assert "rr+intra:///?nodename=server5&service=test_service" in url
            connect_err_called[0] = True
            connect_err_called_evt.set()

        # Pass an invalid URL and make sure error is called
        sub = client_node.SubscribeService(
            "rr+intra:///?nodename=server5&service=test_service")

        sub.ClientConnectFailed += connect_err_handler

        connect_err_called_evt.wait(15)
        assert connect_err_called[0]

        sub.ClientConnectFailed -= connect_err_handler
        time.sleep(0.1)

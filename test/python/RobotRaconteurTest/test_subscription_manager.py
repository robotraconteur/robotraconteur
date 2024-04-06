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


def test_sub_manager_subscribe_by_type():

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

        sub_manager = RR.ServiceSubscriptionManager(node=client_node)

        sub1_details = RR.ServiceSubscriptionManagerDetails(
            Name="sub1",
            ServiceTypes=["experimental.sub_test.testobj"]
        )
        sub_manager.AddSubscription(sub1_details)

        sub = sub_manager.GetSubscription("sub1")
        c = sub.GetDefaultClientWait(5)
        assert c.add_two_numbers(1, 2) == 3

        sub_manager.DisableSubscription("sub1", True)
        time.sleep(0.1)
        res, _ = sub.TryGetDefaultClient()
        assert not res
        sub_manager.EnableSubscription("sub1")
        res, _ = sub.TryGetDefaultClientWait(5)
        assert res

        sub2 = sub_manager.GetSubscription("sub1")
        assert sub is sub2

        sub_manager.RemoveSubscription("sub1", True)
        time.sleep(0.1)
        res, _ = sub.TryGetDefaultClient()
        assert not res

        with pytest.raises(RR.InvalidArgumentException):
            sub_manager.GetSubscription("sub1")

        sub_manager.AddSubscription(sub1_details)

        sub = sub_manager.GetSubscription("sub1")
        c = sub.GetDefaultClientWait(5)
        assert c.add_two_numbers(1, 2) == 3

        print(sub_manager.SubscriptionNames)
        print(sub_manager.SubscriptionDetails)

        sub_manager.Close()


def test_sub_manager_subscribe_by_url():

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

        sub_manager = RR.ServiceSubscriptionManager(node=client_node)

        sub1_details = RR.ServiceSubscriptionManagerDetails(
            Name="sub1",
            Urls=["rr+intra:///?nodename=server1&service=test_service"]

        )
        sub_manager.AddSubscription(sub1_details)

        sub = sub_manager.GetSubscription("sub1")
        c = sub.GetDefaultClientWait(5)
        assert c.add_two_numbers(1, 2) == 3

        sub_manager.DisableSubscription("sub1", True)
        time.sleep(0.1)
        res, _ = sub.TryGetDefaultClient()
        assert not res
        sub_manager.EnableSubscription("sub1")
        res, _ = sub.TryGetDefaultClientWait(5)
        assert res

        sub2 = sub_manager.GetSubscription("sub1")
        assert sub is sub2

        sub_manager.RemoveSubscription("sub1", True)
        time.sleep(0.1)
        res, _ = sub.TryGetDefaultClient()
        assert not res

        with pytest.raises(RR.InvalidArgumentException):
            sub_manager.GetSubscription("sub1")

        sub_manager.AddSubscription(sub1_details)

        sub = sub_manager.GetSubscription("sub1")
        c = sub.GetDefaultClientWait(5)
        assert c.add_two_numbers(1, 2) == 3

        sub_manager.Close()

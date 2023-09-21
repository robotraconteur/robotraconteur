import RobotRaconteur as RR
import pytest
import time

_local_test_service_def = """
service experimental.local_test

object testobj
    function double add_two_numbers(double a, double b)
end object
"""


class _testobj_impl():

    def add_two_numbers(self, a, b):
        return a + b


def test_local_transport():

    local_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT \
        | RR.RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER \
        | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
        | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
        | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

    local_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT \
        | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
        | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

    node1 = RR.RobotRaconteurNode()
    node2 = RR.RobotRaconteurNode()

    node1.Init()
    node2.Init()

    node1_setup = RR.RobotRaconteurNodeSetup(node_name="testprog_local_server", tcp_port=0, flags=local_server_flags,
                                             node=node1)
    node2_setup = RR.RobotRaconteurNodeSetup(
        node_name="", tcp_port=0, flags=local_client_flags, node=node2)

    node1.RegisterServiceType(_local_test_service_def)

    obj = _testobj_impl()
    ctx = node1.RegisterService(
        "test_service", "experimental.local_test.testobj", obj)

    with node1_setup, node2_setup:

        # wait a moment for transport to register
        time.sleep(1)

        url = "rr+local:///?nodename=testprog_local_server&service=test_service"
        node1_nodeid = node1.NodeID
        node1_nodeid_str = node1_nodeid.ToString('D')
        url2 = f"rr+local:///?nodeid={node1_nodeid_str}&service=test_service"

        c = node2.ConnectService(url)
        assert c.add_two_numbers(1, 2) == 3
        node2.DisconnectService(c)

        c2 = node2.ConnectService(url2)
        assert c2.add_two_numbers(3, 2) == 5
        node2.DisconnectService(c2)

        sub = node2.SubscribeServiceByType("experimental.local_test.testobj")
        c = sub.GetDefaultClientWait(1)
        assert c.add_two_numbers(1, 2) == 3
        sub.Close()

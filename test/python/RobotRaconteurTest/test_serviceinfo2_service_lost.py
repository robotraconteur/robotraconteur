import RobotRaconteur as RR
import threading
import time

# Regression test: when a node stops advertising one of its services, ServiceInfo2Subscription must fire
# ServiceLost for it. ServiceInfo2Subscription::NodeUpdated removed the service but fired ServiceDetected.

_robdef = """
service experimental.sub_lost_test

object testobj
    function double add_two_numbers(double a, double b)
end
"""

_server_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


class _testobj_impl(object):
    def add_two_numbers(self, a, b):
        return a + b


def _wait(cond, timeout):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if cond():
            return True
        time.sleep(0.1)
    return cond()


def test_serviceinfo2_service_lost_event():
    server_id = RR.NodeID("6b8a6f2e-2f0c-4c55-9a67-3b2d8f6f0e11")
    server = RR.RobotRaconteurNode()
    server.SetNodeID(server_id)
    server.Init()
    server.RegisterServiceType(_robdef)
    server.RegisterService("service1", "experimental.sub_lost_test.testobj", _testobj_impl())
    server.RegisterService("service2", "experimental.sub_lost_test.testobj", _testobj_impl())
    server_setup = RR.RobotRaconteurNodeSetup("sub_lost_server", 0, flags=_server_flags, node=server)

    client = RR.RobotRaconteurNode()
    client.Init()
    client_setup = RR.RobotRaconteurNodeSetup("", 0, flags=_client_flags, node=client)

    with client_setup, server_setup:
        sub = client.SubscribeServiceInfo2("experimental.sub_lost_test.testobj")
        id2 = RR.ServiceSubscriptionClientID(server_id, "service2")
        assert _wait(lambda: len(sub.GetDetectedServiceInfo2()) == 2, 15), "both services not detected"

        events = []
        lost_evt = threading.Event()

        def lost_handler(s, client_id, info):
            events.append(("lost", client_id.ServiceName))
            lost_evt.set()

        def detected_handler(s, client_id, info):
            events.append(("detected", client_id.ServiceName))

        sub.ServiceLost += lost_handler
        sub.ServiceDetected += detected_handler

        # The node keeps running and keeps advertising service1; only service2 goes away.
        # CloseService does not change the service state nonce, so register another service to announce the change.
        server.CloseService("service2")
        server.RegisterService("service3", "experimental.sub_lost_test.testobj", _testobj_impl())

        assert _wait(lambda: id2 not in sub.GetDetectedServiceInfo2(), 15), "service2 not removed"
        lost_evt.wait(5)
        sub.Close()

    server.Shutdown()
    client.Shutdown()

    # ServiceDetected is also fired again when a detected service's info changes, so a late update can report
    # service2 as detected before it is removed. It must not be reported detected after it was lost
    lost = [n for (k, n) in events if k == "lost"]
    assert lost == ["service2"], "ServiceLost not fired for service2 (events={0})".format(events)
    after_lost = events[events.index(("lost", "service2")) + 1:]
    assert ("detected", "service2") not in after_lost, \
        "ServiceDetected fired for service2 after ServiceLost (events={0})".format(events)

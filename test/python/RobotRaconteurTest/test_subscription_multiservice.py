import RobotRaconteur as RR
import time

# Regression test for a bug in ServiceSubscription::NodeUpdated where a `return`
# (instead of `continue`) aborted the loop over a node's advertised services as
# soon as an already-connected service was encountered. A service registered on a
# node after the subscription had already connected to another service on that same
# node would therefore never be picked up, depending on service-index iteration order.

_robdef = """
service com.robotraconteur.testing.submulti

object sub_testroot
    property double d1
end
"""


class _impl(object):
    def __init__(self):
        self.d1 = 1.0


def _init_node(nodename, first_svc):
    n = RR.RobotRaconteurNode()
    n.Init()
    n.SetNodeName(nodename)
    t = RR.IntraTransport(n)
    n.RegisterTransport(t)
    t.StartServer()
    n.RegisterServiceType(_robdef)
    n.RegisterService(
        first_svc, "com.robotraconteur.testing.submulti.sub_testroot", _impl())
    return n


def _init_client():
    n = RR.RobotRaconteurNode()
    n.Init()
    t = RR.IntraTransport(n)
    n.RegisterTransport(t)
    t.StartClient()
    n.SetLogLevelFromString("WARNING")
    return n


def _wait_count(sub, target, timeout):
    deadline = time.time() + timeout
    last = -1
    while time.time() < deadline:
        last = len(sub.GetConnectedClients())
        if last >= target:
            return last
        time.sleep(0.2)
    return last


def _run_dynamic_add(nodename, first_svc, second_svc):
    server = _init_node(nodename, first_svc)
    time.sleep(0.4)
    client = _init_client()
    try:
        sub = client.SubscribeServiceByType(
            ["com.robotraconteur.testing.submulti.sub_testroot"])
        n1 = _wait_count(sub, 1, 10)
        assert n1 == 1, "expected 1 connected client initially, got {0}".format(n1)

        # Register a second service on the SAME already-running node. The
        # subscription must connect to it in addition to the first service.
        server.RegisterService(
            second_svc, "com.robotraconteur.testing.submulti.sub_testroot", _impl())
        n2 = _wait_count(sub, 2, 15)
        assert n2 == 2, \
            "second service on the node was not subscribed (got {0} connected clients)".format(n2)
        sub.Close()
    finally:
        server.Shutdown()
        client.Shutdown()


def test_subscription_dynamic_second_service_a_first():
    # Already-connected service sorts before the newly added one
    _run_dynamic_add("submulti_node1", "aaa_svc", "zzz_svc")


def test_subscription_dynamic_second_service_z_first():
    # Already-connected service sorts after the newly added one (the order that
    # triggered the original return-instead-of-continue bug)
    _run_dynamic_add("submulti_node2", "zzz_svc", "aaa_svc")

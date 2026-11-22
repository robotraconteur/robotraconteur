import RobotRaconteur as RR
import time

_robdef = """
service experimental.skel_reinit_broadcaster

object testobj
    pipe double p1 [readonly]
    wire double w1 [readonly]
    wire double w2 [writeonly]
end
"""


class _testobj_impl(object):
    pass


def test_skel_reinit_keeps_broadcasters():
    # The same Python object initialized by a second skel keeps the broadcaster/unicast receiver API
    server = RR.RobotRaconteurNode()
    server.Init()
    server.SetNodeName("skel_reinit_broadcaster_server")
    t1 = RR.IntraTransport(server)
    server.RegisterTransport(t1)
    t1.StartServer()
    server.RegisterServiceType(_robdef)
    obj = _testobj_impl()
    server.RegisterService("service1", "experimental.skel_reinit_broadcaster.testobj", obj)
    assert isinstance(obj.p1, RR.PipeBroadcaster)
    assert isinstance(obj.w1, RR.WireBroadcaster)
    assert isinstance(obj.w2, RR.WireUnicastReceiver)
    server.RegisterService("service2", "experimental.skel_reinit_broadcaster.testobj", obj)
    assert isinstance(obj.p1, RR.PipeBroadcaster)
    assert isinstance(obj.w1, RR.WireBroadcaster)
    assert isinstance(obj.w2, RR.WireUnicastReceiver)

    client = RR.RobotRaconteurNode()
    client.Init()
    t2 = RR.IntraTransport(client)
    client.RegisterTransport(t2)
    try:
        c = client.ConnectService("rr+intra:///?nodename=skel_reinit_broadcaster_server&service=service2")
        w = c.w1.Connect()
        obj.w1.OutValue = 3.5
        for _ in range(500):
            if w.InValueValid:
                break
            time.sleep(0.01)
        assert w.InValue == 3.5
        w.Close()
        client.DisconnectService(c)
    finally:
        client.Shutdown()
        server.Shutdown()

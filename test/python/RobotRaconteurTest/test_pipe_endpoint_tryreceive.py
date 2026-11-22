import RobotRaconteur as RR
import time

_robdef = """
service experimental.pipe_endpoint_tryreceive

object testobj
    pipe double p1
end
"""


class _testobj_impl(object):
    def __init__(self):
        self.server_eps = []

    def RRServiceObjectInit(self, ctx, service_path):
        self.p1.PipeConnectCallback = self._connected

    def _connected(self, ep):
        self.server_eps.append(ep)


def test_pipe_endpoint_tryreceivepacketwait():
    server = RR.RobotRaconteurNode()
    server.Init()
    server.SetNodeName("pipe_endpoint_tryreceive_server")
    t1 = RR.IntraTransport(server)
    server.RegisterTransport(t1)
    t1.StartServer()
    server.RegisterServiceType(_robdef)
    obj = _testobj_impl()
    server.RegisterService("service1", "experimental.pipe_endpoint_tryreceive.testobj", obj)

    client = RR.RobotRaconteurNode()
    client.Init()
    t2 = RR.IntraTransport(client)
    client.RegisterTransport(t2)
    try:
        c = client.ConnectService("rr+intra:///?nodename=pipe_endpoint_tryreceive_server&service=service1")
        ep = c.p1.Connect(-1)
        for _ in range(500):
            if len(obj.server_eps) > 0:
                break
            time.sleep(0.01)
        assert len(obj.server_eps) == 1

        # Empty queue: no packet
        res, packet = ep.TryReceivePacketWait(0)
        assert not res
        assert packet is None

        obj.server_eps[0].SendPacket(2.5)
        obj.server_eps[0].SendPacket(3.5)
        # Wait until both packets have arrived before checking Available
        for _ in range(500):
            if ep.Available >= 2:
                break
            time.sleep(0.01)
        res, packet = ep.TryReceivePacketWait(5, True)
        assert res and packet == 2.5
        assert ep.Available == 2
        res, packet = ep.TryReceivePacketWait(5)
        assert res and packet == 2.5
        res, packet = ep.TryReceivePacketWait(5)
        assert res and packet == 3.5
        assert ep.Available == 0
        ep.Close()
        client.DisconnectService(c)
    finally:
        client.Shutdown()
        server.Shutdown()

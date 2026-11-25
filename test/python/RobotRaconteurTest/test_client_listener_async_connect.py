import RobotRaconteur as RR
import threading
import time

_robdef = """
service experimental.client_listener_async_connect

object testobj
    property double d1
end
"""


class _testobj_impl(object):
    def __init__(self):
        self.d1 = 1.0


def test_client_listener_async_connect():
    # The listener of an async connected client must be called with the client object reference
    server = RR.RobotRaconteurNode()
    server.Init()
    server.SetNodeName("client_listener_async_connect_server")
    t1 = RR.IntraTransport(server)
    server.RegisterTransport(t1)
    t1.StartServer()
    server.RegisterServiceType(_robdef)
    server.RegisterService("service1", "experimental.client_listener_async_connect.testobj", _testobj_impl())

    client = RR.RobotRaconteurNode()
    client.Init()
    t2 = RR.IntraTransport(client)
    client.RegisterTransport(t2)
    try:
        events = []

        def listener(stub, code, param):
            events.append((stub, code))

        connected = threading.Event()
        res = []

        def handler(stub, err):
            res.append((stub, err))
            connected.set()

        client.AsyncConnectService("rr+intra:///?nodename=client_listener_async_connect_server&service=service1",
                                   None, None, listener, handler, 5)
        assert connected.wait(10)
        c, err = res[0]
        assert err is None
        assert c.d1 == 1.0

        client.DisconnectService(c)
        for _ in range(500):
            if len(events) > 0:
                break
            time.sleep(0.01)
        assert len(events) > 0
        for stub, code in events:
            assert stub is c
        assert RR.ClientServiceListenerEventType_ClientClosed in [e[1] for e in events]
    finally:
        client.Shutdown()
        server.Shutdown()

import RobotRaconteur as RR
import time

_require_tls_test_service_def = """
service experimental.tcp_require_tls_test

object testobj
    property double d1
end
"""


class _testobj_impl:
    def __init__(self):
        self.d1 = 0.0


def test_tcp_require_tls_rejects_plaintext_client():
    # With RequireTls set, a plaintext client must not be connected to the service:
    # the first non-STARTTLS message closes the transport and must not be processed

    server_node = RR.RobotRaconteurNode()
    server_node.Init()
    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    try:
        server_transport = RR.TcpTransport(server_node)
        server_transport.RequireTls = True
        server_transport.StartServer(0, True)
        server_node.RegisterTransport(server_transport)
        port = server_transport.GetListenPort()

        server_node.RegisterServiceType(_require_tls_test_service_def)
        ctx = server_node.RegisterService(
            "test_service", "experimental.tcp_require_tls_test.testobj", _testobj_impl())

        connected = []

        def listener(context, code, endpoint):
            if code == RR.ServerServiceListenerEventType_ClientConnected:
                connected.append(endpoint)

        ctx.AddServerServiceListener(listener)

        client_transport = RR.TcpTransport(client_node)
        client_node.RegisterTransport(client_transport)

        url = "rr+tcp://127.0.0.1:%d/?service=test_service" % port
        for _ in range(3):
            try:
                c = client_node.ConnectService(url)
                try:
                    client_node.DisconnectService(c)
                except Exception:
                    pass
            except Exception:
                pass

        time.sleep(0.5)
        assert len(connected) == 0
    finally:
        client_node.Shutdown()
        server_node.Shutdown()

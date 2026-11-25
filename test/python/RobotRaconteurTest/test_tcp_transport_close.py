import RobotRaconteur as RR
import time

_close_test_service_def = """
service experimental.tcp_close_test

object testobj
    property double d1
end
"""


class _testobj_impl:
    def __init__(self):
        self.d1 = 0.0


def test_tcp_transport_close_does_not_wait_for_closed_connections():
    # TcpTransport.Close() waits up to 500 ms only for connections that are still open.
    # Connections that closed immediately must not make it wait.

    server_node = RR.RobotRaconteurNode()
    server_node.Init()
    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    try:
        server_transport = RR.TcpTransport(server_node)
        server_transport.StartServer(0, True)
        server_node.RegisterTransport(server_transport)
        port = server_transport.GetListenPort()

        server_node.RegisterServiceType(_close_test_service_def)
        server_node.RegisterService(
            "test_service", "experimental.tcp_close_test.testobj", _testobj_impl())

        client_transport = RR.TcpTransport(client_node)
        client_node.RegisterTransport(client_transport)

        url = "rr+tcp://127.0.0.1:%d/?service=test_service" % port
        c = client_node.ConnectService(url)
        c.d1 = 1.0

        t0 = time.time()
        client_transport.Close()
        elapsed = time.time() - t0
        assert elapsed < 0.4, "TcpTransport.Close() took %f s" % elapsed
        del c
    finally:
        client_node.Shutdown()
        server_node.Shutdown()

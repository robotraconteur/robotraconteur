import RobotRaconteur as RR
import time

_experimental_distfiles_test = """
service experimental.distfiles_test

object DistfilesTest
    property double c
    function double add(double a, int32 b)
end
"""


class DistfilesTest_impl(object):
    def __init__(self):
        self.c = 0

    def add(self, a, b):
        return a + b + self.c


def main():
    node = RR.RobotRaconteurNode()
    node.Init()
    node.RegisterServiceType(_experimental_distfiles_test)

    client_node = RR.RobotRaconteurNode()
    client_node.Init()

    try:
        server_tcp_transport = RR.TcpTransport(node)
        server_tcp_transport.StartServer(0)
        client_tcp_transport = RR.TcpTransport(client_node)
        node.RegisterTransport(server_tcp_transport)
        client_node.RegisterTransport(client_tcp_transport)

        listen_port = server_tcp_transport.GetListenPort()

        obj = DistfilesTest_impl()
        node.RegisterService(
            "DistfilesTest", "experimental.distfiles_test.DistfilesTest", obj)
        time.sleep(0.5)

        c = client_node.ConnectService(
            f"rr+tcp://localhost:{listen_port}?service=DistfilesTest")

        c.c = 5
        assert (c.add(1, 2) == 8)

        print("Test complete")

    finally:
        node.Shutdown()
        client_node.Shutdown()


if __name__ == '__main__':
    main()

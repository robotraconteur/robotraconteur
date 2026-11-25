import RobotRaconteur as RR
import socket
import threading
import time


def test_tcp_starttls_no_response_reports_error():
    # A server that accepts the TCP connection but never answers the STARTTLS request.
    # The client STARTTLS timer (5 s) must fail the connect attempt, instead of
    # leaving it to the (here much longer) connect timeout

    listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listen_sock.bind(("127.0.0.1", 0))
    listen_sock.listen(5)
    port = listen_sock.getsockname()[1]

    accepted = []
    stop = threading.Event()

    def server_thread():
        listen_sock.settimeout(0.2)
        while not stop.is_set():
            try:
                s, _ = listen_sock.accept()
                s.settimeout(0.2)
                accepted.append(s)
            except socket.timeout:
                pass
            except OSError:
                return
            for s in list(accepted):
                try:
                    s.recv(4096)
                except Exception:
                    pass

    t = threading.Thread(target=server_thread)
    t.daemon = True
    t.start()

    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    try:
        client_transport = RR.TcpTransport(client_node)
        client_transport.DefaultConnectTimeout = 30
        client_node.RegisterTransport(client_transport)

        url = "rrs+tcp://127.0.0.1:%d/?nodeid=0d694574-1ad8-4b9e-9aea-e881524fb451&service=test_service" % port

        done = threading.Event()
        result = []

        def handler(obj, err):
            result.append(err)
            done.set()

        t0 = time.time()
        client_node.AsyncConnectService(url, None, None, None, handler, 60000)
        assert done.wait(45)
        elapsed = time.time() - t0
        assert result[0] is not None
        assert elapsed < 15, "STARTTLS failure not reported, connect took %f s" % elapsed
    finally:
        client_node.Shutdown()
        stop.set()
        t.join()
        listen_sock.close()
        for s in accepted:
            s.close()

import RobotRaconteur as RR
import socket


def _ws_handshake(port, origin):
    s = socket.create_connection(("127.0.0.1", port), timeout=5)
    try:
        req = ("GET / HTTP/1.1\r\n"
               "Host: 127.0.0.1:%d\r\n"
               "Upgrade: websocket\r\n"
               "Connection: Upgrade\r\n"
               "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
               "Sec-WebSocket-Version: 13\r\n"
               "Sec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\n"
               "Origin: %s\r\n\r\n") % (port, origin)
        s.sendall(req.encode("ascii"))
        resp = b""
        while b"\r\n" not in resp:
            d = s.recv(4096)
            if not d:
                break
            resp += d
        return resp.split(b"\r\n")[0].decode("ascii", errors="replace")
    finally:
        s.close()


def test_tcp_ws_rejected_handshake_then_close():
    # After a rejected handshake the socket is released while the accept state can still be alive for a moment.
    # Closing the transport in that window closed the released socket through the websocket stream (use after
    # free; crashed intermittently on Windows, found by AddressSanitizer on Linux)
    for _ in range(50):
        node = RR.RobotRaconteurNode()
        node.Init()
        try:
            t = RR.TcpTransport(node)
            t.StartServer(0, True)
            node.RegisterTransport(t)
            port = t.GetListenPort()
            for origin in ("chrome-extension://x", "http://forbidden.example", "https://forbidden.example"):
                assert " 403 " in _ws_handshake(port, origin)
        finally:
            node.Shutdown()

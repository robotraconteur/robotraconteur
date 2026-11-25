import RobotRaconteur as RR
import socket


def _ws_handshake_status(port, origin):
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


def test_tcp_ws_default_origins():
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        t = RR.TcpTransport(node)
        t.StartServer(0, True)
        node.RegisterTransport(t)
        port = t.GetListenPort()

        assert " 101 " in _ws_handshake_status(port, "https://robotraconteur.com")
        # Pages loaded from local files send "null"; allowed by default
        assert " 101 " in _ws_handshake_status(port, "null")
        # Any browser extension, or plain http origins an on-path attacker can produce, are not allowed by default
        for origin in ["chrome-extension://abcdefghijklmnop", "http://robotraconteur.com",
                       "http://www.robotraconteur.com"]:
            assert " 403 " in _ws_handshake_status(port, origin), origin
        # "null" can be removed and added back explicitly (the origin regex used to reject it)
        t.RemoveWebSocketAllowedOrigin("null")
        assert " 403 " in _ws_handshake_status(port, "null")
        t.AddWebSocketAllowedOrigin("null")
        assert " 101 " in _ws_handshake_status(port, "null")
    finally:
        node.Shutdown()

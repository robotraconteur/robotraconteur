import os
import subprocess
import sys
import time

import pytest

import RobotRaconteur as RR

_client_script = r'''
import sys
import RobotRaconteur as RR
port = int(sys.argv[1])
cli = RR.RobotRaconteurNode()
cli.Init()
ct = RR.TcpTransport(cli)
cli.RegisterTransport(ct)
n = 0
for i in range(3):
    try:
        cli.ConnectService("rrs+tcp://127.0.0.1:%d/?service=nosuch" % port)
    except Exception:
        n += 1
print("FAILED_CONNECTS %d" % n)
cli.Shutdown()
'''


def _count_sockets():
    n = 0
    for f in os.listdir("/proc/self/fd"):
        try:
            if os.readlink("/proc/self/fd/" + f).startswith("socket:"):
                n += 1
        except OSError:
            pass
    return n


@pytest.mark.skipif(not sys.platform.startswith("linux"), reason="uses /proc")
def test_tcp_starttls_without_server_certificate_closes_connection():
    # A server without a node certificate answers a STARTTLS request with an error.
    # The connection must then be closed, not left paused forever.
    server_node = RR.RobotRaconteurNode()
    server_node.Init()
    try:
        server_transport = RR.TcpTransport(server_node)
        server_transport.StartServer(0)
        server_node.RegisterTransport(server_transport)
        port = server_transport.GetListenPort()
        time.sleep(0.5)

        before = _count_sockets()

        p = subprocess.Popen([sys.executable, "-c", _client_script, str(port)], stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        out, _ = p.communicate()
        out = out.decode("utf-8", "replace")
        assert "FAILED_CONNECTS 3" in out, out

        # The client process has exited, so all of its connections are gone
        after = _count_sockets()
        t0 = time.time()
        while after > before and time.time() - t0 < 10:
            time.sleep(0.1)
            after = _count_sockets()
        assert after <= before, "server leaked %d sockets after STARTTLS without certificate" % (after - before)
    finally:
        server_node.Shutdown()

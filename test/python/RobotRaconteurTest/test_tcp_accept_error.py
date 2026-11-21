import os
import subprocess
import sys

import pytest

_accept_error_script = r'''
import os
import resource
import socket
import time
import RobotRaconteur as RR

robdef = """
service experimental.tcp_accept_error_test

object testobj
    property double d1
end
"""


class testobj_impl:
    def __init__(self):
        self.d1 = 0.0


server_node = RR.RobotRaconteurNode()
server_node.Init()
server_transport = RR.TcpTransport(server_node)
server_transport.StartServer(0, True)
server_node.RegisterTransport(server_transport)
port = server_transport.GetListenPort()
server_node.RegisterServiceType(robdef)
server_node.RegisterService("test_service", "experimental.tcp_accept_error_test.testobj", testobj_impl())

client_node = RR.RobotRaconteurNode()
client_node.Init()
client_transport = RR.TcpTransport(client_node)
client_node.RegisterTransport(client_transport)
time.sleep(0.5)

soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
nopen = len(os.listdir("/proc/self/fd"))
resource.setrlimit(resource.RLIMIT_NOFILE, (nopen + 16, hard))

# Use up all file descriptors, keeping one socket to connect with, so the
# server's accept() fails with EMFILE
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
fill = []
try:
    while True:
        fill.append(os.open("/dev/null", os.O_RDONLY))
except OSError:
    pass
s.connect(("127.0.0.1", port))
time.sleep(1)
for f in fill:
    os.close(f)
s.close()
resource.setrlimit(resource.RLIMIT_NOFILE, (soft, hard))
time.sleep(1)

# The server must still accept connections
c = client_node.ConnectService("rr+tcp://127.0.0.1:%d/?service=test_service" % port)
c.d1 = 1.0
client_node.DisconnectService(c)
client_node.Shutdown()
server_node.Shutdown()
print("ACCEPT_OK")
'''


@pytest.mark.skipif(not sys.platform.startswith("linux"), reason="uses /proc and RLIMIT_NOFILE")
@pytest.mark.skipif(sys.version_info < (3, 5), reason="needs subprocess.run (Python 3.5+)")
def test_tcp_accept_continues_after_accept_error():
    p = subprocess.run([sys.executable, "-c", _accept_error_script], stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT, timeout=120)
    out = p.stdout.decode(errors="replace")
    assert p.returncode == 0 and "ACCEPT_OK" in out, out

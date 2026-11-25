import os
import subprocess
import sys
import tempfile
import shutil

import pytest

_local_accept_error_script = r'''
import os
import resource
import socket
import time
import RobotRaconteur as RR

robdef = """
service experimental.local_accept_error_test

object testobj
    property double d1
end
"""


class testobj_impl:
    def __init__(self):
        self.d1 = 0.0


node_name = "experimental.local_accept_error_test"

server_node = RR.RobotRaconteurNode()
server_node.Init()
server_transport = RR.LocalTransport(server_node)
server_transport.StartServerAsNodeName(node_name)
server_node.RegisterTransport(server_transport)
server_node.RegisterServiceType(robdef)
server_node.RegisterService("test_service", "experimental.local_accept_error_test.testobj", testobj_impl())

client_node = RR.RobotRaconteurNode()
client_node.Init()
client_transport = RR.LocalTransport(client_node)
client_node.RegisterTransport(client_transport)

sock_files = []
for root, dirs, files in os.walk(os.environ["RR_TEST_RUN_BASE"]):
    for f in files:
        if f.endswith(".sock") and os.path.basename(root) == "socket":
            sock_files.append(os.path.join(root, f))
assert len(sock_files) == 1, sock_files

soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
nopen = len(os.listdir("/proc/self/fd"))
resource.setrlimit(resource.RLIMIT_NOFILE, (nopen + 16, hard))

# Use up all file descriptors, keeping one socket to connect with, so the
# server's accept() fails with EMFILE
s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
fill = []
try:
    while True:
        fill.append(os.open("/dev/null", os.O_RDONLY))
except OSError:
    pass
s.connect(sock_files[0])
time.sleep(1)
for f in fill:
    os.close(f)
s.close()
resource.setrlimit(resource.RLIMIT_NOFILE, (soft, hard))
time.sleep(1)

# The server must still accept connections
c = client_node.ConnectService("rr+local:///?nodename=%s&service=test_service" % node_name)
c.d1 = 1.0
client_node.DisconnectService(c)
client_node.Shutdown()
server_node.Shutdown()
print("ACCEPT_OK")
'''


@pytest.mark.skipif(not sys.platform.startswith("linux"), reason="uses /proc and RLIMIT_NOFILE")
@pytest.mark.skipif(sys.version_info < (3, 5), reason="needs subprocess timeout (Python 3)")
def test_local_accept_continues_after_accept_error():
    base = tempfile.mkdtemp(prefix="rr-test-")
    try:
        env = dict(os.environ)
        env["RR_TEST_RUN_BASE"] = base
        env["ROBOTRACONTEUR_USER_RUN_DIR"] = os.path.join(base, "user")
        env["ROBOTRACONTEUR_SYSTEM_RUN_DIR"] = os.path.join(base, "system")
        p = subprocess.Popen([sys.executable, "-c", _local_accept_error_script], stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, env=env)
        try:
            out, _ = p.communicate(timeout=120)
        except subprocess.TimeoutExpired:
            p.kill()
            out, _ = p.communicate()
        out = out.decode("utf-8", "replace")
        assert p.returncode == 0 and "ACCEPT_OK" in out, out
    finally:
        shutil.rmtree(base, ignore_errors=True)

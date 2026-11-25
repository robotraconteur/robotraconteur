import RobotRaconteur as RR

_robdef = """
service experimental.scoped_monitor_lock

object testobj
    property double d1
end
"""


class _testobj_impl(object):
    def __init__(self):
        self.d1 = 1.0
        self.enter_count = 0
        self.exit_count = 0

    def RobotRaconteurMonitorEnter(self, timeout):
        self.enter_count += 1

    def RobotRaconteurMonitorExit(self):
        self.exit_count += 1


def test_scoped_monitor_lock():
    server = RR.RobotRaconteurNode()
    server.Init()
    server.SetNodeName("scoped_monitor_lock_server")
    t1 = RR.IntraTransport(server)
    server.RegisterTransport(t1)
    t1.StartServer()
    server.RegisterServiceType(_robdef)
    obj = _testobj_impl()
    server.RegisterService("service1", "experimental.scoped_monitor_lock.testobj", obj)

    client = RR.RobotRaconteurNode()
    client.Init()
    t2 = RR.IntraTransport(client)
    client.RegisterTransport(t2)
    try:
        c = client.ConnectService("rr+intra:///?nodename=scoped_monitor_lock_server&service=service1")

        # Plain use
        with client.ScopedMonitorLock(c):
            assert c.d1 == 1.0
        assert (obj.enter_count, obj.exit_count) == (1, 1)

        # __enter__ returns the lock; unlock() inside the block must not be followed by a second
        # MonitorExit when the block exits
        with client.ScopedMonitorLock(c) as l:
            assert l is not None
            assert l.locked
            l.unlock()
            assert not l.locked
            l.lock(-1)
            l.unlock()
        assert (obj.enter_count, obj.exit_count) == (3, 3)

        # release(): the lock stays held when the block exits
        with client.ScopedMonitorLock(c) as l:
            l.release()
        assert (obj.enter_count, obj.exit_count) == (4, 3)
        client.MonitorExit(c)
        assert (obj.enter_count, obj.exit_count) == (4, 4)

        # The object can be locked again
        with client.ScopedMonitorLock(c):
            pass

        client.DisconnectService(c)
    finally:
        client.Shutdown()
        server.Shutdown()

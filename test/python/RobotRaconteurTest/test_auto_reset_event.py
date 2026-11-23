import threading
import time

import RobotRaconteur as RR


def test_auto_reset_event_wait_one_seconds():
    # WaitOne takes the timeout in seconds, like the other timeouts of the Python API
    e = RR.AutoResetEvent()
    t0 = time.monotonic()
    assert not e.WaitOne(0.2)
    dt = time.monotonic() - t0
    assert 0.15 <= dt < 5

    e.Set()
    assert e.WaitOne(1)
    assert not e.WaitOne(0)

    # No timeout: wait until set
    threading.Timer(0.1, e.Set).start()
    assert e.WaitOne()

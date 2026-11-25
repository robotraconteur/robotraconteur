import RobotRaconteur as RR
import sys
import pytest


@pytest.mark.skipif(sys.version_info < (3, 6), reason="awaitable form requires asyncio")
def test_discovery_async_future():
    # With handler=None the async find functions return an awaitable future that must resolve
    # to the result list
    import asyncio
    node = RR.RobotRaconteurNode()
    node.Init()
    t = RR.IntraTransport(node)
    node.RegisterTransport(t)
    loop = asyncio.new_event_loop()
    try:
        asyncio.set_event_loop(loop)

        def run(f):
            return loop.run_until_complete(asyncio.wait_for(f(), 10))

        res = run(lambda: node.AsyncFindNodeByName("discovery_async_future_none", ["rr+intra"], None, 0.1))
        assert isinstance(res, list)
        res = run(lambda: node.AsyncFindNodeByID(RR.NodeID("a7e4ca1f-6d0b-4b83-a8b3-5e2b3c6f7e10"),
                                                 ["rr+intra"], None, 0.1))
        assert isinstance(res, list)
        res = run(lambda: node.AsyncFindServiceByType("experimental.discovery_async_future.none",
                                                      ["rr+intra"], None, 0.1))
        assert isinstance(res, list)
    finally:
        asyncio.set_event_loop(None)
        loop.close()
        node.Shutdown()

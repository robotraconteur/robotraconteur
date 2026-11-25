import RobotRaconteur as RR
import gc
import time

_client_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT \
    | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE


def _count_directors(type_name):
    gc.collect()
    return sum(1 for o in gc.get_objects() if type(o).__name__ == type_name)


def _wait_count(type_name, count):
    for _ in range(100):
        if _count_directors(type_name) == count:
            return True
        time.sleep(0.01)
    return False


def test_subscription_director_release():
    wire_dir = "WrappedWireSubscriptionDirectorPython"
    pipe_dir = "WrappedPipeSubscriptionDirectorPython"
    wire_count = _count_directors(wire_dir)
    pipe_count = _count_directors(pipe_dir)

    client_node = RR.RobotRaconteurNode()
    client_node.Init()
    client_setup = RR.RobotRaconteurNodeSetup(
        "", 0, flags=_client_flags, node=client_node)
    with client_setup:
        sub = client_node.SubscribeServiceByType(
            "experimental.subscription_director_release.testobj")
        wire_sub = sub.SubscribeWire("testwire")
        pipe_sub = sub.SubscribePipe("testpipe")
        assert _count_directors(wire_dir) == wire_count + 1
        assert _count_directors(pipe_dir) == pipe_count + 1

        # Closing a member subscription releases its director
        wire_sub.Close()
        assert _wait_count(wire_dir, wire_count)

        # Closing the service subscription releases the directors of its member subscriptions
        sub.Close()
        assert _wait_count(pipe_dir, pipe_count)
        del wire_sub
        del pipe_sub

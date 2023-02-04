from RobotRaconteur.Client import *
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
from .ServiceTest2 import ServiceTest2_fill_teststruct3, ServiceTest2_verify_teststruct3
from .ServiceTestClient2 import ServiceTestClient2
import sys


def test_servicetest2_types():
    if sys.version_info < (3, 0):
        return
    node = RR.RobotRaconteurNode()
    node.Init()

    try:
        node.RegisterServiceTypesFromFiles(["com.robotraconteur.testing.TestService2", "com.robotraconteur.testing.TestService1",
                                            "com.robotraconteur.testing.TestService3", "com.robotraconteur.testing.TestService5"])
        s1 = ServiceTest2_fill_teststruct3(node, 1000, None)
        s1_m = PackMessageElement(
            s1, 'com.robotraconteur.testing.TestService3.teststruct3', node=node)
        s1_m.UpdateData()
        s1_1 = UnpackMessageElement(
            s1_m, 'com.robotraconteur.testing.TestService3.teststruct3', node=node)
        ServiceTest2_verify_teststruct3(s1_1, 1000)

    finally:
        node.Shutdown()


def _member_test(fn, test_server_node_config):
    service_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService2")
    c = ServiceTestClient2()
    c.ConnectService(service_url)
    try:
        fn(c)
    finally:
        c.DisconnectService()


def test_wire_peek_poke(test_server_node_config):
    _member_test(lambda c: c.TestWirePeekPoke(), test_server_node_config)


def test_enums(test_server_node_config):
    _member_test(lambda c: c.TestEnums(), test_server_node_config)


def test_pods(test_server_node_config):
    _member_test(lambda c: c.TestPods(), test_server_node_config)


def test_memories(test_server_node_config):
    if sys.version_info < (3, 0):
        return
    _member_test(lambda c: c.TestMemories(), test_server_node_config)


def test_generators(test_server_node_config):
    _member_test(lambda c: c.TestGenerators(), test_server_node_config)


def test_named_arrays(test_server_node_config):
    _member_test(lambda c: c.TestNamedArrays(), test_server_node_config)


def test_named_array_memmories(test_server_node_config):
    _member_test(lambda c: c.TestNamedArrayMemories(), test_server_node_config)


def test_complex(test_server_node_config):
    _member_test(lambda c: c.TestComplex(), test_server_node_config)


def test_complex_memories(test_server_node_config):
    _member_test(lambda c: c.TestComplexMemories(), test_server_node_config)


def test_nolock(test_server_node_config):
    _member_test(lambda c: c.TestNoLock(), test_server_node_config)


def test_bools(test_server_node_config):
    _member_test(lambda c: c.TestBools(), test_server_node_config)


def test_bool_memories(test_server_node_config):
    _member_test(lambda c: c.TestBoolMemories(), test_server_node_config)


def test_exception_params(test_server_node_config):
    _member_test(lambda c: c.TestExceptionParams(), test_server_node_config)

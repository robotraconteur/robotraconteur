from RobotRaconteur.Client import *
from .ServiceTestClient3 import ServiceTestClient3


def _member_test(fn, test_server_node_config):
    service_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService3")
    c = ServiceTestClient3()
    c.ConnectService(service_url)
    try:
        fn(c)
    finally:
        c.DisconnectService()


def test_properties3(test_server_node_config):
    _member_test(lambda c: c.TestProperties(), test_server_node_config)


def test_functions3(test_server_node_config):
    _member_test(lambda c: c.TestFunctions(), test_server_node_config)

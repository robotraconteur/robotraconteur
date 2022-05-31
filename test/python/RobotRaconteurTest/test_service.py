import pytest
from .ServiceTestClient import ServiceTestClient
import os
from RobotRaconteur.Client import *
import sys


def _member_test(fn, test_server_node_config):
    service_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService")
    c = ServiceTestClient()
    c.ConnectService(service_url)
    try:
        fn(c)
    finally:
        c.DisconnectService()


def test_properties(test_server_node_config):
    _member_test(lambda c: c.TestProperties(), test_server_node_config)


def test_functions(test_server_node_config):
    _member_test(lambda c: c.TestFunctions(), test_server_node_config)


def test_events(test_server_node_config):
    _member_test(lambda c: c.TestEvents(), test_server_node_config)


def test_objrefs(test_server_node_config):
    _member_test(lambda c: c.TestObjRefs(), test_server_node_config)


def test_pipes(test_server_node_config):
    _member_test(lambda c: c.TestPipes(), test_server_node_config)


def test_callbacks(test_server_node_config):
    _member_test(lambda c: c.TestCallbacks(), test_server_node_config)


def test_wires(test_server_node_config):
    _member_test(lambda c: c.TestWires(), test_server_node_config)


def test_memories(test_server_node_config):
    if sys.version_info < (3, 0):
        return
    _member_test(lambda c: c.TestMemories(), test_server_node_config)


def test_authentication(test_server_node_config):
    service_auth_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService_auth")
    c = ServiceTestClient()
    c.TestAuthentication(service_auth_url)


def test_object_lock(test_server_node_config):
    service_auth_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService_auth")
    c = ServiceTestClient()
    c.TestObjectLock(service_auth_url)


def test_monitor_lock(test_server_node_config):
    service_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService")
    c = ServiceTestClient()
    c.TestMonitorLock(service_url)


def test_async(test_server_node_config):
    service_auth_url = test_server_node_config.get_service_url(
        "RobotRaconteurTestService_auth")
    c = ServiceTestClient()
    c.TestAsync(service_auth_url)

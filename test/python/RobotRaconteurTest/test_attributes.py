import pytest
from .ServiceTestClient import ServiceTestClient
import os
from RobotRaconteur.Client import *
import sys


def test_attributes(test_server_node_config):
    c = RRN.ConnectService(test_server_node_config.get_service_url("RobotRaconteurTestService"))

    attr = RRN.GetServiceAttributes(c)

    assert (len(attr) == 2)

    assert (attr["test"].data == "This is a test attribute")
    assert (attr["test2"].data[0] == 42)

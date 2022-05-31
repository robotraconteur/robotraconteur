import os
from RobotRaconteurTest.service_test_util import TestServerNodeConfig
import pytest


def pytest_generate_tests(metafunc):
    robdef_path = os.path.abspath(os.path.dirname(
        os.path.abspath(__file__)) + "/../robdef")
    os.environ["ROBOTRACONTEUR_ROBDEF_PATH"] = robdef_path
    os.environ["ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC"] = "TRUE"
    os.environ["ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY"] = "TRUE"
    os.environ["ROBOTRACONTEUR_LOG_LEVEL"] = "ERROR"


@pytest.fixture(scope="session")
def test_server_node_config():
    return TestServerNodeConfig("unit_test_service_python")

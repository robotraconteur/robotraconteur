import os

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.parent / "python"))
from RobotRaconteurTest.service_test_util import TestServerNodeConfig
import pytest

def pytest_generate_tests(metafunc):
    robdef_path = os.path.abspath(os.path.dirname(
        os.path.abspath(__file__)) + "/../robdef")
    os.environ["ROBOTRACONTEUR_ROBDEF_PATH"] = robdef_path
    os.environ["ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC"] = "TRUE"
    # os.environ["ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY"] = "TRUE"
    os.environ["ROBOTRACONTEUR_LOG_LEVEL"] = "ERROR"




@pytest.fixture(scope="session")
def test_server_node_config():
    
    test_node_config = TestServerNodeConfig("unit_test_service_python")
    try:        
        yield test_node_config
    finally:
        test_node_config.shutdown()

@pytest.fixture(scope="function")
def test_server_node_config_data(selenium,test_server_node_config):
    print(selenium.base_url)
    test_server_node_config.node_setup.tcp_transport.AddWebSocketAllowedOrigin(selenium.base_url)
    data = {"url": test_server_node_config.get_service_url("RobotRaconteurTestService")}
    return data
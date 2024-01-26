import pytest
import os
import sys

from pytest_pyodide import run_in_pyodide

@run_in_pyodide(packages=["RobotRaconteur", "numpy"])
async def test_client_basic(selenium, test_server_node_config_data):
    from RobotRaconteur.Client import RRN, RR
    import pytest
    service_url = test_server_node_config_data["url"]
    c = await RRN.AsyncConnectService(service_url,None,None,None,handler=None)
    try:
        await c.async_set_d1(3.456,None)
        assert (await c.async_get_d1(None)) == 12.345

        with pytest.raises(Exception):
            await c.async_set_errtest(1,None)

        await c.async_func1(None)
        await c.async_func2(10, 20.34, None)
        assert (await c.async_func3(2, 3.45,None)) == 5.45
        assert (await c.async_meaning_of_life(None)) == 42

        with pytest.raises(Exception):
            await c.async_func_errtest(None)

        with pytest.raises(RR.DataTypeException):
            await c.async_func_errtest1(None)
    finally:
        await RRN.AsyncDisconnectService(c,None)
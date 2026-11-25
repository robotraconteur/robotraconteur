import RobotRaconteur as RR
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement
import numpy as np
import pytest


@pytest.mark.parametrize("type_str,n", [
    ("double[3] value", 5),
    ("double[3] value", 2),
    ("double[3-] value", 4),
    ("double value", 2),
])
def test_pack_numpy_wrong_length(type_str, n):
    # A numpy array is checked against the declared length like a list
    with pytest.raises(RR.DataTypeException):
        PackMessageElement([0.0] * n, type_str)
    with pytest.raises(RR.DataTypeException):
        PackMessageElement(np.zeros(n), type_str)


@pytest.mark.parametrize("type_str,n", [
    ("double[3] value", 3),
    ("double[3-] value", 2),
    ("double[] value", 7),
    ("double value", 1),
])
def test_pack_numpy_valid_length(type_str, n):
    PackMessageElement(np.zeros(n), type_str)
    if type_str != "double value":
        PackMessageElement(np.zeros((n, 1)), type_str)

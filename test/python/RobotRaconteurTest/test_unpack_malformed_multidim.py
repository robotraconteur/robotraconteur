from RobotRaconteur.Client import *
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import numpy as np
import pytest


def _find_element(elements, name):
    return RRP.MessageElement.FindElement(elements, name)


def _multidim_with_dims(dims, type_str):
    m = PackMessageElement(np.zeros((2, 2), dtype=np.float64), type_str)
    l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())
    dims_el = _find_element(l.Elements, "dims")
    dims_el.SetData(PackMessageElement(np.array(dims, dtype=np.uint32), "uint32[] dims").GetData())
    m.UpdateData()
    return m


@pytest.mark.parametrize("type_str", ["double[*] value", "varvalue value"], ids=["double", "varvalue"])
def test_unpack_multidim_dims_larger_than_array(type_str):
    # The product of the wire dims must match the number of array elements, otherwise the numpy array
    # would be created over the wire buffer with the larger dims (out-of-bounds read)
    m = _multidim_with_dims([100, 100], "double[*] value")
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(m, type_str)


@pytest.mark.parametrize("type_str", ["double[*] value", "varvalue value"], ids=["double", "varvalue"])
def test_unpack_multidim_dims_smaller_than_array(type_str):
    m = _multidim_with_dims([1, 2], "double[*] value")
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(m, type_str)


def test_unpack_multidim_valid():
    a = np.arange(6, dtype=np.float64).reshape((2, 3), order="F")
    m = PackMessageElement(a, "double[*] value")
    b = UnpackMessageElement(m, "double[*] value")
    np.testing.assert_array_equal(a, b)

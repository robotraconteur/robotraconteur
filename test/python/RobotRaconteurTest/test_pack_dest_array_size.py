import RobotRaconteur as RR
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackToRRArray
import numpy as np
import pytest


def _type(s):
    t = RRP.TypeDefinition()
    t.FromString(s)
    return t


@pytest.mark.parametrize("type_str,dtype,values", [
    ("double[] a", np.float64, [1.0, 2.0, 3.0, 4.0, 5.0]),
    ("int32[] a", np.int32, [1, 2, 3, 4, 5]),
    ("cdouble[] a", np.complex128, [1j, 2j, 3j, 4j, 5j]),
    ("bool[] a", np.bool_, [True, False, True, False, True]),
], ids=["double", "int32", "cdouble", "bool"])
def test_pack_list_into_dest_array_size_mismatch(type_str, dtype, values):
    # A list longer than the caller-supplied destination array must be rejected, not written past
    # the end of the destination buffer
    t = _type(type_str)
    dest = PackToRRArray(np.zeros(4, dtype=dtype), t)
    with pytest.raises(RR.DataTypeException):
        PackToRRArray(values, t, dest)
    with pytest.raises(RR.DataTypeException):
        PackToRRArray(values[0:3], t, dest)
    dest2 = PackToRRArray(values[0:4], t, dest)
    assert dest2.size() == 4


def test_pack_bytearray_into_dest_array():
    t = _type("uint8[] a")
    dest = PackToRRArray(np.zeros(4, dtype=np.uint8), t)
    # Matching size is accepted
    PackToRRArray(bytearray(b"\x01\x02\x03\x04"), t, dest)
    # Larger or smaller is rejected (larger would overflow the destination)
    with pytest.raises(RR.DataTypeException):
        PackToRRArray(bytearray(6), t, dest)
    with pytest.raises(RR.DataTypeException):
        PackToRRArray(bytearray(2), t, dest)

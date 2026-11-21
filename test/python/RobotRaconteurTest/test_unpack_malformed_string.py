from RobotRaconteur.Client import *
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import ctypes
import pytest
import sys


def _invalidate_utf8(string_element):
    # Overwrite the first byte of a packed string with 0xFF, which is never valid UTF-8
    a = RRP.MessageElementDataUtil.ToRRBaseArray(string_element.GetData())
    assert a.size() > 0
    ctypes.memset(int(a.void_ptr()), 0xFF, 1)


@pytest.mark.skipif(sys.version_info < (3, 0), reason="Python 2 strings are byte strings, UTF-8 is not decoded")
def test_unpack_invalid_utf8_string():
    m = PackMessageElement("abc", "string value")
    _invalidate_utf8(m)
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(m, "string value")


@pytest.mark.skipif(sys.version_info < (3, 0), reason="Python 2 strings are byte strings, UTF-8 is not decoded")
@pytest.mark.parametrize("data,type_str", [
    ({"a": "abc", "b": "def"}, "string{string} value"),
    ({1: "abc", 2: "def"}, "string{int32} value"),
    (["abc", "def"], "string{list} value"),
], ids=["map_string", "map_int32", "list"])
def test_unpack_invalid_utf8_string_in_container(data, type_str):
    # An invalid UTF-8 value in a container returned NULL without an exception, and the NULL was
    # inserted into the dict/list (crash)
    m = PackMessageElement(data, type_str)
    l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())
    _invalidate_utf8(l.Elements[1])
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(m, type_str)


@pytest.mark.parametrize("data,type_str", [
    ({"a": "abc", "b": "def"}, "string{string} value"),
    ({1: "abc", 2: "def"}, "string{int32} value"),
], ids=["map_string", "map_int32"])
def test_unpack_valid_string_map(data, type_str):
    m = PackMessageElement(data, type_str)
    assert UnpackMessageElement(m, type_str) == data

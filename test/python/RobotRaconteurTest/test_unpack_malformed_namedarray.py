from RobotRaconteur.Client import *
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import numpy as np
import pytest

_robdef = """service experimental.bughunt_py_namedarray
stdver 0.10

namedarray vec3
    field double x
    field double y
    field double z
end
"""


def _find_element(elements, name):
    return RRP.MessageElement.FindElement(elements, name)


def test_unpack_varvalue_namedarray_wrong_element_type():
    # A namedarray received in a varvalue must have the numeric type of the namedarray. Otherwise the
    # numpy array would be built over a wire buffer that is smaller than the dtype requires.
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        dt = node.GetNamedArrayDType("experimental.bughunt_py_namedarray.vec3")
        v = np.zeros((1,), dtype=dt)
        m = PackMessageElement(RR.VarValue(v, "experimental.bughunt_py_namedarray.vec3[]"),
                               "varvalue value", node=node)
        l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())
        inner = _find_element(l.Elements, "array")
        # Same element count (3) but 1 byte per element instead of 8
        inner.SetData(RRP.AllocateRRArrayByType(RRP.DataTypes_int8_t, 3))
        inner.ElementType = RRP.DataTypes_int8_t
        m.UpdateData()

        with pytest.raises(RR.DataTypeException):
            UnpackMessageElement(m, "varvalue value", node=node)
    finally:
        node.Shutdown()

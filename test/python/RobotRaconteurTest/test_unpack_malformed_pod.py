from RobotRaconteur.Client import *
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import numpy as np
import pytest

_robdef = """service experimental.bughunt_py_pod
stdver 0.10

pod testpod
    field double s
    field double[3-] a
    field double[2,2] b
end
"""

_pod_type = "experimental.bughunt_py_pod.testpod"


def _find_element(elements, name):
    return RRP.MessageElement.FindElement(elements, name)


@pytest.mark.parametrize("field_name", ["a", "b"])
def test_unpack_pod_numeric_array_field_sent_as_string(field_name):
    # A pod field declared as a numeric array must not be accepted from the wire as another element type.
    # The unpacked value was cast to PyArrayObject* without a type check.
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        dt = node.GetPodDType(_pod_type)
        v = np.zeros((1,), dtype=dt)
        m = PackMessageElement(v, _pod_type + " value", node=node)
        l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())
        pod_el = l.Elements[0]
        pod_l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(pod_el.GetData())
        f = _find_element(pod_l.Elements, field_name)
        f.SetData(PackMessageElement("not a numeric array", "string value").GetData())
        f.ElementType = RRP.DataTypes_string_t
        m.UpdateData()

        with pytest.raises(RR.DataTypeException):
            UnpackMessageElement(m, _pod_type + " value", node=node)
    finally:
        node.Shutdown()


def test_unpack_pod_valid():
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        dt = node.GetPodDType(_pod_type)
        v = np.zeros((1,), dtype=dt)
        v[0]["s"] = 1.5
        v[0]["a"]["len"] = 2
        v[0]["a"]["array"][0:2] = [3, 4]
        v[0]["b"] = [[5, 6], [7, 8]]
        m = PackMessageElement(v, _pod_type + " value", node=node)
        m.UpdateData()
        v2 = UnpackMessageElement(m, _pod_type + " value", node=node)
        assert v2[0]["s"] == 1.5
        assert v2[0]["a"]["len"] == 2
        np.testing.assert_array_equal(v2[0]["a"]["array"][0:2], [3, 4])
        np.testing.assert_array_equal(v2[0]["b"], [[5, 6], [7, 8]])
    finally:
        node.Shutdown()

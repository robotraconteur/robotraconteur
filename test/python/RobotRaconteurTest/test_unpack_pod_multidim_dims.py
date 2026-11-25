import RobotRaconteur as RR
from RobotRaconteur import RobotRaconteurPython as RRP
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import numpy as np
import pytest

_robdef = """
service experimental.unpack_pod_multidim_dims

pod testpod
    field double a
end

namedarray testnamedarray
    field double a
end
"""


@pytest.fixture(scope="module")
def node(request):
    n = RR.RobotRaconteurNode()
    n.Init()
    n.RegisterServiceType(_robdef)
    request.addfinalizer(n.Shutdown)
    return n


def _set_dims(m, dims, node):
    # Replace the "dims" element of a packed pod/namedarray multidim array with the given dims
    l = RRP.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())
    dims_el = RRP.MessageElement.FindElement(l.Elements, "dims")
    dims_el.SetData(PackMessageElement(np.array(dims, dtype=np.uint32), "uint32[] dims").GetData())
    m.UpdateData()


def _packed(node, typename, dims):
    dt = node.GetPodDType("experimental.unpack_pod_multidim_dims." + typename) if typename == "testpod" \
        else node.GetNamedArrayDType("experimental.unpack_pod_multidim_dims." + typename)
    a = np.zeros((2, 2), dtype=dt)
    type_str = "experimental.unpack_pod_multidim_dims." + typename + "[*] value"
    m = PackMessageElement(a, type_str, node=node)
    _set_dims(m, dims, node)
    return m, type_str


@pytest.mark.parametrize("typename", ["testpod", "testnamedarray"])
def test_unpack_pod_multidim_wrong_dims(node, typename):
    # The wire dims do not match the array length. PyArray_Newshape fails; this must be
    # reported as a DataTypeException instead of returning NULL with a numpy error pending.
    m, type_str = _packed(node, typename, [100, 100])
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(m, type_str, node=node)


@pytest.mark.parametrize("typename", ["testpod", "testnamedarray"])
def test_unpack_pod_multidim_wrong_dims_in_list(node, typename):
    # Same inside a list container: the NULL result must not be stored in the list
    m, type_str = _packed(node, typename, [100, 100])
    l = RRP.vectorptr_messageelement()
    m.ElementNumber = 0
    m.ElementFlags = (m.ElementFlags & ~RRP.MessageElementFlags_ELEMENT_NAME_STR) \
        | RRP.MessageElementFlags_ELEMENT_NUMBER
    l.append(m)
    lm = RRP.MessageElement()
    lm.ElementName = "value"
    lm.SetData(RRP.MessageElementNestedElementList(RRP.DataTypes_list_t, "", l))
    lm.UpdateData()
    with pytest.raises(RR.DataTypeException):
        UnpackMessageElement(lm, type_str[:-len(" value")] + "{list} value", node=node)


@pytest.mark.parametrize("typename", ["testpod", "testnamedarray"])
def test_unpack_pod_multidim_valid(node, typename):
    m, type_str = _packed(node, typename, [2, 2])
    b = UnpackMessageElement(m, type_str, node=node)
    assert b.shape == (2, 2)

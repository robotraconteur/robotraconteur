from RobotRaconteur.Client import *
from RobotRaconteur import RobotRaconteurPython as RRP
import pytest

_service_name = "experimental.bughunt_py_recursive"

_robdef = """service experimental.bughunt_py_recursive
stdver 0.10

pod p1
    field double a
    field p2 b
end

pod p2
    field double c
end

namedarray na1
    field double x
    field na2 y
end

namedarray na2
    field double z
end
"""


@pytest.mark.parametrize("entry_name,dtype_func", [("p1", "GetPodDType"), ("na1", "GetNamedArrayDType")])
def test_recursive_pod_dtype(entry_name, dtype_func):
    # Service definitions pulled from a remote service are not verified, so a pod or namedarray
    # may contain itself. Building the numpy dtype must fail instead of recursing until the stack
    # overflows. Simulate the unverified definition by editing the registered definition in place.
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        d = node.GetServiceType(_service_name)
        entries = list(d.Pods) + list(d.NamedArrays)
        e = [x for x in entries if x.Name == entry_name][0]
        p = RRP.MemberDefinitionUtil.ToProperty(e.Members[1])
        t = RRP.TypeDefinition()
        t.FromString(_service_name + "." + entry_name + " " + p.Name)
        p.Type = t

        with pytest.raises(RR.DataTypeException):
            getattr(node, dtype_func)(_service_name + "." + entry_name)
    finally:
        node.Shutdown()


def test_nested_pod_dtype():
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        dt = node.GetPodDType(_service_name + ".p1")
        assert dt.names == ("a", "b")
        dt2 = node.GetNamedArrayDType(_service_name + ".na1")
        assert dt2.names == ("x", "y")
    finally:
        node.Shutdown()

from RobotRaconteur.Client import *
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement
import numpy as np
import pytest

_robdef = """service experimental.bughunt_py_pod_multidim
stdver 0.10

pod testpod
    field double s
end
"""

_pod_type = "experimental.bughunt_py_pod_multidim.testpod"


def test_pack_pod_multidim_not_ndarray():
    # A pod/namedarray multidim array given as something other than a numpy.ndarray must raise
    # DataTypeException. The binding threw a DataTypeException* (pointer), which nothing caught,
    # so the interpreter aborted.
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        with pytest.raises(RR.DataTypeException):
            PackMessageElement([1, 2], _pod_type + "[*] value", None, node)

        dt = node.GetPodDType(_pod_type)
        m = PackMessageElement(np.zeros((2, 2), dtype=dt), _pod_type + "[*] value", None, node)
        assert m is not None
    finally:
        node.Shutdown()

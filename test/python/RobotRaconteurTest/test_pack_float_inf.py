import RobotRaconteur as RR
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement
import numpy as np
import pytest


@pytest.mark.parametrize("type_str", ["double[] value", "single[] value"])
def test_pack_list_inf(type_str):
    # inf and -inf in a Python list are valid floating point values (the numpy path sends them)
    v = [float('inf'), -float('inf'), 1.5]
    m = PackMessageElement(v, type_str)
    b = UnpackMessageElement(m, type_str)
    np.testing.assert_array_equal(b, np.array(v))


def test_pack_list_single_out_of_range():
    # Finite values outside of the single range are still rejected
    with pytest.raises(RR.DataTypeException):
        PackMessageElement([1e300], "single[] value")

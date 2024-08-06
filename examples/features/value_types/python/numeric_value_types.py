# numeric_value_types.py - Example of using numeric value types

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Scalar numbers
# property double a_double
a_double_read = c.a_double
print(a_double_read)
# assert is used for the rest of the example property read operations
# to demonstrate the expected values.
assert (c.a_double == 5.78)
c.a_double = 49.3
# property int32 b_int
assert (c.b_int == 4557)
c.b_int = 359
# property uint8 c_byte
assert (c.c_byte == 0x1A)
c.c_byte = 31
# property cdouble d_cdouble
assert (c.d_cdouble == 23.7 + 1j * 5.3)
c.d_cdouble = 1.2 + 3.4j
# property bool e_bool
assert (c.e_bool == True)
c.e_bool = False
# property int32 meaning_of_life [readonly]
assert (c.meaning_of_life == 42)

# Numeric Arrays
# Note the use of `dtype` parameter when creating arrays. This must match the
# type specified in the service definition.
# property double[] a_double_array
np.testing.assert_allclose(c.a_double_array, [0.016, 0.226])
c.a_double_array = np.array([0.582, 0.288, 0.09, 0.213, 0.98], dtype=np.float64)
# property double[3] a_double_array_fixed
np.testing.assert_allclose(c.a_double_array_fixed, [0.13, 0.27, 0.15])
c.a_double_array_fixed = np.array([0.21, 0.12, 0.39], dtype=np.float64)
# property double[6-] a_double_array_maxlen
np.testing.assert_allclose(c.a_double_array_maxlen, [0.7, 0.16, 0.16, 0.05, 0.61, 0.9])
c.a_double_array_maxlen = np.array([0.035, 0.4], dtype=np.float64)
# property double[3,2] a_double_marray_fixed
np.testing.assert_allclose(c.a_double_marray_fixed, [[0.29, 0.66], [0.41, 0.6], [0.4, 0.2]])
c.a_double_marray_fixed = np.array([[0.3, 0.6], [0.4, 0.6], [0.5, 0.2]], dtype=np.float64)
# property double[*] a_double_marray
np.testing.assert_allclose(c.a_double_marray, [[0.72, 0.4], [0.05, 0.07]])
c.a_double_marray = np.array([[0.3], [0.01]], dtype=np.float64)
# property uint8[] c_byte_array
np.testing.assert_array_equal(c.c_byte_array, [0x1A, 0x2B])
c.c_byte_array = np.array([0x3C, 0x4D, 0x5E, 0x6F, 0x70], dtype=np.uint8)

# namedarray_value_types.py - Example of using namedarray value types

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Retrieve NamedArray numpy dtypes. NamedArrays are represented as NumPy dtypes with fields
# obj is only used for clients. It is omitted for services.
my_vector3_dtype = RRN.GetNamedArrayDType("experimental.value_types.MyVector3", obj=c)
my_quaternion_dtype = RRN.GetNamedArrayDType("experimental.value_types.MyQuaternion", obj=c)
my_pose_dtype = RRN.GetNamedArrayDType("experimental.value_types.MyPose", obj=c)

# NumPy structures are used to represent NamedArrays.
# These structures can also be converted to and from plain arrays of the
# underlying numeric type using RRN.NamedArrayToArray() and RRN.ArrayToNamedArray().
# Scalar NamedArrays are represented as arrays with a single element.

# property MyVector3 g_vector
a = np.zeros((1,), dtype=my_vector3_dtype)
a[0]["x"] = 1.0
a[0]["y"] = 2.0
a[0]["z"] = 3.0
c.g_vector = a

b = c.g_vector
assert (b[0]["x"] == 4.0)
assert (b[0]["y"] == 5.0)
assert (b[0]["z"] == 6.0)

# Use plain arrays and convert to and from NamedArrays
d = np.array([1.0, 2.0, 3.0], dtype=np.float64)
c.g_vector = RRN.ArrayToNamedArray(d, named_array_dt=my_vector3_dtype)
e = RRN.NamedArrayToArray(c.g_vector)
np.testing.assert_array_equal(e, [[4.0, 5.0, 6.0]])

# Now an array of NamedArrays
# property MyVector3[] g_vector_array
f = np.zeros((2,), dtype=my_vector3_dtype)
f[0]["x"] = 1.0
f[0]["y"] = 2.0
f[0]["z"] = 3.0
f[1]["x"] = 4.0
f[1]["y"] = 5.0
f[1]["z"] = 6.0
c.g_vector_array = f

g = c.g_vector_array
assert (g[0]["x"] == 7.0)
assert (g[0]["y"] == 8.0)
assert (g[0]["z"] == 9.0)
assert (g[1]["x"] == 10.0)
assert (g[1]["y"] == 11.0)
assert (g[1]["z"] == 12.0)

# Use plain arrays and convert to and from NamedArrays
h = np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]], dtype=np.float64)
c.g_vector_array = RRN.ArrayToNamedArray(h, named_array_dt=my_vector3_dtype)
i = RRN.NamedArrayToArray(c.g_vector_array)
np.testing.assert_array_equal(i, [[7.0, 8.0, 9.0], [10.0, 11.0, 12.0]])

# property MyQuaternion h_quaternion
j = np.zeros((1,), dtype=my_quaternion_dtype)
j[0]["w"] = 1.0
j[0]["x"] = 0.0
j[0]["y"] = 0.0
j[0]["z"] = 0.0
c.h_quaternion = j

k = c.h_quaternion
assert (k[0]["w"] == 0.707)
assert (k[0]["x"] == 0.0)
assert (k[0]["y"] == 0.707)
assert (k[0]["z"] == 0.0)

# MyPose is a composite NamedArray with fields of other NamedArrays
# property MyPose l_pose
l = np.zeros((1,), dtype=my_pose_dtype)
l[0]["orientation"]["w"] = 1.0
l[0]["orientation"]["x"] = 0.0
l[0]["orientation"]["y"] = 0.0
l[0]["orientation"]["z"] = 0.0
l[0]["position"]["x"] = 1.0
l[0]["position"]["y"] = 2.0
l[0]["position"]["z"] = 3.0
c.i_pose = l

m = c.i_pose
assert (m[0]["orientation"]["w"] == 0.707)
assert (m[0]["orientation"]["x"] == 0.0)
assert (m[0]["orientation"]["y"] == 0.707)
assert (m[0]["orientation"]["z"] == 0.0)
assert (m[0]["position"]["x"] == 4.0)
assert (m[0]["position"]["y"] == 5.0)
assert (m[0]["position"]["z"] == 6.0)

# Use plain arrays and convert to and from NamedArrays
n = np.array([1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0,], dtype=np.float64)
c.i_pose = RRN.ArrayToNamedArray(n, named_array_dt=my_pose_dtype)
o = RRN.NamedArrayToArray(c.i_pose)
np.testing.assert_array_equal(o, [[0.707, 0.0, 0.707, 0.0, 4.0, 5.0, 6.0]])

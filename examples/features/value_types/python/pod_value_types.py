# pod_value_types.py - Example of using pod value types

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Example using Pod numpy dtypes. Pods are represented as NumPy dtypes with fields
# obj is only used for clients. It is omitted for services.
my_pod_dtype = RRN.GetPodDType("experimental.value_types.MyPod", obj=c)

# property MyPod j_pod
a = np.zeros((1,), dtype=my_pod_dtype)
a[0]["a"] = 0.928
a[0]["b"] = 8374
a[0]["c"] = [8, 9, 10, 11]
# Max length fields are stored with a length field and
# data field.
a[0]["d"]["len"] = 1
a[0]["d"]["array"][0] = 17
a[0]["e"] = [[1, 2, 3], [4, 5, 6]]
a[0]["f"]["x"] = 10.1
a[0]["f"]["y"] = 10.2
a[0]["f"]["z"] = 10.3
c.j_pod = a

b = c.j_pod
assert (b[0]["a"] == 0.791)
assert (b[0]["b"] == 1077)
np.testing.assert_array_equal(b[0]["c"], [61, 52, 33, 24])
assert (b[0]["d"]["len"] == 2)
np.testing.assert_array_equal(b[0]["d"]["array"][0:2], [1, 2])
np.testing.assert_array_equal(b[0]["e"], [[7, 8, 9], [10, 11, 12]])
assert (b[0]["f"]["x"] == 20.1)
assert (b[0]["f"]["y"] == 20.2)
assert (b[0]["f"]["z"] == 20.3)

# Now an array of Pods
# property MyPod[] j_pod_array
d = np.zeros((2,), dtype=my_pod_dtype)
d[0]["a"] = 0.928
d[0]["b"] = 8374
d[0]["c"] = [8, 9, 10, 11]
d[0]["d"]["len"] = 1
d[0]["d"]["array"][0] = 17
d[0]["e"] = [[1, 2, 3], [4, 5, 6]]
d[0]["f"]["x"] = 10.1
d[0]["f"]["y"] = 10.2
d[0]["f"]["z"] = 10.3
d[1]["a"] = 0.67
d[1]["b"] = 123
d[1]["c"] = [1, 2, 3, 4]
d[1]["d"]["len"] = 2
d[1]["d"]["array"][0:2] = [5, 6]
d[1]["e"] = [[13, 14, 15], [16, 17, 18]]
d[1]["f"]["x"] = 30.1
d[1]["f"]["y"] = 30.2
d[1]["f"]["z"] = 30.3
c.j_pod_array = d

e = c.j_pod_array
assert (e[0]["a"] == 0.791)
assert (e[0]["b"] == 1077)
np.testing.assert_array_equal(e[0]["c"], [61, 52, 33, 24])
assert (e[0]["d"]["len"] == 2)
np.testing.assert_array_equal(e[0]["d"]["array"][0:2], [1, 2])
np.testing.assert_array_equal(e[0]["e"], [[7, 8, 9], [10, 11, 12]])
assert (e[0]["f"]["x"] == 20.1)
assert (e[0]["f"]["y"] == 20.2)
assert (e[0]["f"]["z"] == 20.3)
assert (e[1]["a"] == 0.03)
assert (e[1]["b"] == 693)
np.testing.assert_array_equal(e[1]["c"], [5, 6, 7, 8])
assert (e[1]["d"]["len"] == 1)
np.testing.assert_array_equal(e[1]["d"]["array"][0:1], [3])
np.testing.assert_array_equal(e[1]["e"], [[19, 20, 21], [22, 23, 24]])
assert (e[1]["f"]["x"] == 40.1)
assert (e[1]["f"]["y"] == 40.2)
assert (e[1]["f"]["z"] == 40.3)

# container_value_types.py - Example of using container value types (maps and lists)

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# property double{int32} l_double_map
# int32 keyed map of scalar doubles
c.l_double_map = {
    1: 1.1,
    2: 2.2,
    5: 3.3
}

b = c.l_double_map
assert (len(b) == 2)
assert (b[1] == 5.1)
assert (b[5] == 6.2)

# property double[]{string} l_double_array_map
# string keyed map of double arrays
c.l_double_array_map = {
    "key1": np.array([1.1, 1.2, 1.3], dtype=np.float64),
    "key2": np.array([2.1, 2.2, 2.3], dtype=np.float64)
}

d = c.l_double_array_map
assert (len(d) == 2)
np.testing.assert_array_equal(d["key3"], [5.1, 5.2, 5.3])
np.testing.assert_array_equal(d["key4"], [6.1, 6.2, 6.3])

# property string{list} m_string_list
# list of strings
c.m_string_list = ["string 1", "string 2", "string 3"]

a = c.m_string_list
assert (len(a) == 2)
assert (a[0] == "string 4")
assert (a[1] == "string 5")


# property string{int32} m_string_map_int32
# int32 keyed map of strings
c.m_string_map_int32 = {
    12: "string 1",
    100: "string 2",
    17: "string 3"
}

e = c.m_string_map_int32
assert (len(e) == 2)
assert (e[87] == "string 4")
assert (e[1] == "string 5")

# property string{string} m_string_map_string
# string keyed map of strings
c.m_string_map_string = {
    "key1": "string 1",
    "key2": "string 2"
}

f = c.m_string_map_string
assert (len(f) == 3)
assert (f["key3"] == "string 3")
assert (f["key4"] == "string 4")
assert (f["key5"] == "string 5")

# property MyVector3{int32} n_vector_map
# int32 keyed map of MyVector3
my_vector3_dtype = RRN.GetNamedArrayDType("experimental.value_types.MyVector3", obj=c)
e = np.zeros((1,), dtype=my_vector3_dtype)
e[0]["x"] = 1.0
e[0]["y"] = 2.0
e[0]["z"] = 3.0
c.n_vector_map = {
    1: e
}

g = c.n_vector_map
assert (len(g) == 1)
g1 = g[1]
assert (g1[0]["x"] == 4.0)
assert (g1[0]["y"] == 5.0)
assert (g1[0]["z"] == 6.0)

# property MyStructure{list} o_struct_list
# list of MyStructure
my_structure_type = RRN.GetStructureType("experimental.value_types.MyStructure", obj=c)
h = my_structure_type()
h.a = 5
h.b = np.array([10, 20], dtype=np.uint32)
h.c = "String from structure client"
h.d = [
    "string a",
    "string b"
]
c.o_struct_list = [h]

i = c.o_struct_list
assert (len(i) == 1)
i0 = i[0]
assert (i0.a == 52)
np.testing.assert_array_equal(i[0].b, np.array([110, 120], dtype=np.uint32))
assert (i0.c == "String from structure service")
assert (len(i[0].d) == 3)
assert (i0.d[0] == "string c")
assert (i0.d[1] == "string d")
assert (i0.d[2] == "string e")

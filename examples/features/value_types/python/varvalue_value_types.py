# varvalue_value_types.py - Example of using varvalue wildcard type

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Any valid Robot Raconteur data type can be stored in a varvalue
# In Python, the RR.VarValue() Python structure is used to store
# the value and type of the data. The type is stored as a string.
# This is necessary because Python does not carry enough type
# information

# property varvalue p_varvalue_double_array
# Example of storing a double array in a varvalue
a = np.array([1.1, 2.2, 3.3], dtype=np.float64)
c.p_varvalue_double_array = RR.VarValue(a, "double[]")
b = c.p_varvalue_double_array
np.testing.assert_array_equal(b.data, [1.4, 2.5, 3.6])
assert b.datatype == "double[]"

# property varvalue q_varvalue_string
# Example of storing a string in a varvalue
c.q_varvalue_string = RR.VarValue("varvalue string from client", "string")
d = c.q_varvalue_string
assert d.data == "varvalue string from service"
assert d.datatype == "string"

# property varvalue r_varvalue_struct
# Example of storing a structure in a varvalue
my_structure_type = RRN.GetStructureType("experimental.value_types.MyStructure", obj=c)
s = my_structure_type()
s.a = 5
s.b = np.array([10, 20], dtype=np.uint32)
s.c = "String from structure client"
s.d = [
    "string a",
    "string b"
]

c.r_varvalue_struct = RR.VarValue(s, "experimental.value_types.MyStructure")

t = c.r_varvalue_struct
t_data = t.data

assert t_data.a == 52
np.testing.assert_array_equal(t_data.b, np.array([110, 120], dtype=np.uint32))
assert t_data.c == "String from structure service"
assert len(t_data.d) == 3
assert t_data.d[0] == "string c"
assert t_data.d[1] == "string d"
assert t_data.d[2] == "string e"

# property varvalue{string} s_varvalue_map2
# Example of storing a map of strings in a varvalue
# varvalue{string} is used extensively in structures
# to allow for additional fields to be added to the
# structure without changing the structure definition
c.s_varvalue_map2 = {
    "key1": RR.VarValue(np.array([2, 3]), "int32[]"),
    "key2": RR.VarValue("string 2", "string")
}

u = c.s_varvalue_map2
assert len(u) == 2
np.testing.assert_array_equal(u["key3"].data, [4, 5])
assert u["key3"].datatype == "int32[]"
assert u["key4"].data == "string 4"
assert u["key4"].datatype == "string"

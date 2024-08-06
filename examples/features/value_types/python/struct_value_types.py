# struct_value_type_simple.py - Simple example of using struct value types

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Example using SimpleStructure type. Also see example
# using MyStructure for a more complex example.

# Retrieve the structure type. obj is only used
# for clients. It is omitted for services.
simple_structure_type = RRN.GetStructureType("experimental.value_types.MyStructure", obj=c)

# Create and populate a SimpleStructure
s = simple_structure_type()
# field double a
s.a = 5
# field uint32[2] b
s.b = np.array([10, 20], dtype=np.uint32)
# field string c
s.c = "String from structure client"
# field string{list} d
s.d = [
    "string a",
    "string b"
]

# Set the property using the structure
# property SimpleStructure k_struct
c.k_struct = s

# Retrieve the structure from the service
u = c.k_struct

# Structures can be None
assert u is not None

# field double a
assert (u.a == 52)
# field uint32[2] b
np.testing.assert_array_equal(u.b, np.array([110, 120], dtype=np.uint32))
# field string c
assert (u.c == "String from structure service")
# field string{list} d
assert (len(u.d) == 3)
assert (u.d[0] == "string c")
assert (u.d[1] == "string d")
assert (u.d[2] == "string e")

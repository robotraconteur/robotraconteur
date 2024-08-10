# null_value_types.py - Example of types that can be null

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Null values
# Null values are "None" in Python
# struct, containers, and varvalue are nullable.
# Numbers, arrays, strings, pods, and namedarrays are not nullable.

# property MyStructure t_struct_null
c.t_struct_null = None
assert c.t_struct_null is None

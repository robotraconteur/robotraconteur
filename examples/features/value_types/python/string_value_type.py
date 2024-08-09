# string_value_type.py - Example of using string value types

from RobotRaconteur.Client import *
import numpy as np

c = RRN.ConnectService('rr+tcp://localhost:53223?service=values_example')

# Strings
# property string f_string
f_string_read = c.f_string
print(f_string_read)
# assert is used for the rest of the example property read operations
# to demonstrate the expected values.
assert (c.f_string == "An example string read from the service")
c.f_string = "An example string written to the service"

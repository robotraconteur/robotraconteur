# exception_example.py - Example of using exceptions in Python

from RobotRaconteur.Client import *

# Connect to the service
url = 'rr+tcp://localhost:53224/?service=exception_example'
c = RRN.ConnectService(url)

# Call the functions

# Catch an OperationFailedException built-in exception
try:
    c.my_exception_function1()
except RR.OperationFailedException as e:
    print("Caught OperationFailedException: " + str(e))

# Catch a custom exception
exp_type = RRN.GetExceptionType("experimental.exception_example.MyExampleCustomException", c)
try:
    c.my_exception_function2()
except exp_type as e:
    print("Caught MyExampleCustomException: " + str(e))

# Catch RobotRaconteurException to catch all Robot Raconteur exceptions
try:
    c.my_exception_function1()
except RR.RobotRaconteurException as e:
    print("Caught RobotRaconteurException: " + str(e))

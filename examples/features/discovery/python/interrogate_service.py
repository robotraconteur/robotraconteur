# interrogate_service.py - Interrogate a service on the network to get information about it
# and the object members it contains.

# This example demonstrates finding all available nodes on the network using the special
# "Service Index" node that all Robot Raconteur nodes provide. It then reads the service definition
# of one of the services and prints out information about the object members it contains.

from RobotRaconteur.Client import *
import time

# Root object type of the special service index service
# See https://github.com/robotraconteur/robotraconteur/blob/master/RobotRaconteurCore/src/RobotRaconteurServiceIndex.robdef
service_type = "RobotRaconteurServiceIndex.ServiceIndex"

# Transports to search, in this case TCP and local
transports = ["rr+tcp", "rr+local"]

# After initializing the node, wait a few seconds for discovery to initialize

time.sleep(6)

detected_services = []

ret = RRN.FindServiceByType(service_type, transports)

for s in ret:
    c = RRN.ConnectService(s.ConnectionURL)
    c_detected_services = c.GetLocalNodeServices()
    detected_services.extend(list(c_detected_services.values()))
    RRN.DisconnectService(c)

print(detected_services)

# Find a service that implements the "experimental.reynard_the_robot.Reynard" type
reynard_service_info = None
for s in detected_services:
    if s.RootObjectType == "experimental.reynard_the_robot.Reynard":
        reynard_service_info = s
        break

if reynard_service_info is None:
    print("Reynard service not found")
    exit(1)

print(reynard_service_info)

# Connect to the service
c_reynard = RRN.ConnectService(list(reynard_service_info.ConnectionURL.values()))

# Get the root object type
object_type = RRN.GetObjectType(c_reynard)

from RobotRaconteur.RobotRaconteurPythonUtil import SplitQualifiedName

# Split the qualified type into service type and object type name
service_type, object_type_name = SplitQualifiedName(object_type)
print("Service type: " + service_type)
print("Object type name: " + object_type_name)

# Get the service definition pulled from the service
service_def = RRN.GetPulledServiceType(c_reynard, service_type)

print()
print(service_def.ToString())

root_object_def = None
# Find the root object in the service definition
for o in service_def.Objects:
    if o.Name == object_type_name:
        root_object_def = o
        break

if root_object_def is None:
    print("Root object definition not found")
    exit(1)

# All members can now be interrogated. Let's look at the functions

for i in range(len(root_object_def.Members)):
    member = root_object_def.Members[i]
    if (isinstance(member, RR.FunctionDefinition)):
        f = member
        print("Function: " + f.Name)
        print("  Return type: " + f.ReturnType.ToString())
        print("  Parameters:")
        for p in f.Parameters:
            print("    " + p.ToString())
        print()

# Dynamically call a function

say_func = getattr(c_reynard, "say")
say_params = ["Hello from interrogate_service!"]
say_func(*say_params)

# See https://github.com/robotraconteur/robotraconteur/blob/master/RobotRaconteurCore/include/RobotRaconteur/ServiceDefinition.h
# for more information on service definitions and object members
# See https://github.com/robotraconteur/robotraconteur/blob/5c1705259ae5807457d28a85299c4e0e2c8e7610/RobotRaconteurPython/RobotRaconteurPythonUtil.py#L219:L420
# for an advanced example using interrogation to create a dynamic proxy object

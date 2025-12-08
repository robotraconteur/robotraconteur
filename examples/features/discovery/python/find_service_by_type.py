# find_service_by_type.py - Find a service on the network using discovery by type

# Find a service by "type" on the network. A "service type" is defined by the fully qualified
# type of the root object of the service. For example, the Reynard the Robot service has an object
# type "Reynard" in service "experimental.reynard_the_robot", so the fully qualified type is
# "experimental.reynard_the_robot.Reynard".

# Root object type to find
service_type = "experimental.reynard_the_robot.Reynard"

# Transports to search, in this case TCP and local
transports = ["rr+tcp", "rr+local"]

from RobotRaconteur.Client import *
import time

# After initializing the node, wait a few seconds for discovery to initialize

time.sleep(6)

ret = RRN.FindServiceByType(service_type, transports)

for s in ret:
    print("Name: " + s.Name)
    print("RootObjectType: " + s.RootObjectType)
    print("RootObjectImplements: " + str(s.RootObjectImplements))
    print("ConnectionURL: " + str(s.ConnectionURL))
    print("Attributes: " + str(s.Attributes))
    print("NodeID: " + str(s.NodeID))
    print("NodeName: " + s.NodeName)
    print("")

# Use the URL to connect to the service

c = RRN.ConnectService(ret[0].ConnectionURL)
c.say("Hello from find_service_by_type!")

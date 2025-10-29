# find_node_by_name.py - Find a node by "name" on the network.
# Robot Raconteur nodes can be identified by a "node name" or a "node ID". The node
# name is a human readable name that may or may not be unique. The Node ID is a 128 bit
# uuid that is statistically expected to be unique. This example shows how to find a node
# by its node name.

# The node name to find
node_name = "experimental.reynard_the_robot"

# Transports to search, in this case TCP and local
transports = ["rr+tcp", "rr+local"]

from RobotRaconteur.Client import *
import time

# After initializing the node, wait a few seconds for discovery to initialize

time.sleep(6)

ret = RRN.FindNodeByName(node_name, transports)

for s in ret:
    print("NodeID: " + str(s.NodeID))
    print("NodeName: " + s.NodeName)
    # The connection URL returned is a normal Robot Raconteur connection URL
    # without the "service" parameter. Multiple URLs may be returned
    # if there are multiple paths to the node
    print("ConnectionURL: " + str(s.ConnectionURL))

# Use the URL to connect to a service on the node

url2 = [u + "&service=reynard" for u in ret[0].ConnectionURL]

c = RRN.ConnectService(url2)
c.say("Hello from find_node_by_name!")

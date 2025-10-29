# find_node_by_id.py - Find a node by "ID" on the network.
# Robot Raconteur nodes can be identified by a "node name" or a "node ID". The node
# name is a human readable name that may or may not be unique. The Node ID is a 128 bit
# uuid that is statistically expected to be unique. This example shows how to find a node
# by its node ID.

from RobotRaconteur.Client import *
import time

# The node name to find
# Note that this Node ID is unique to each instance of Reynard the Robot
# and will need to be changed to your local instance's Node ID
# This information is returned by the find_node_by_name example
node_id = RR.NodeID("122e8c01-1053-456c-8b23-9cda781a91f7")

# Transports to search, in this case TCP and local
transports = ["rr+tcp", "rr+local"]


# After initializing the node, wait a few seconds for discovery to initialize

time.sleep(6)

ret = RRN.FindNodeByID(node_id, transports)

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
c.say("Hello from find_node_by_id!")

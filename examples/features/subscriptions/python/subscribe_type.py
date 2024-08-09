# subscribe_type.py - Subscribe to services by type

from RobotRaconteur.Client import *
import time

# Subscribe to a service by type
root_object_type = "experimental.reynard_the_robot.Reynard"
sub = RRN.SubscribeServiceByType(root_object_type)
# Give a few seconds for the service to be connected
time.sleep(6)

# Run a function on all connected services. Could also use GetDefaultClientWait() to
# get the first connected client and run functions on that client
# without waiting for all clients to connect
connected_clients = sub.GetConnectedClients()
for k, c in connected_clients.items():
    print(f"Saying hello on client {k}")
    c.say("Hello from type subscription!")

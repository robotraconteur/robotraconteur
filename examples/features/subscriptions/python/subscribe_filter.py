# subscribe_filter.py - Subscribe to services by type using a filter

from RobotRaconteur.Client import *
import time


def subscription_predicate(service_info2):
    # This function is called for each discovered service
    # Return True to connect to the service, False to ignore
    # The service_info2 parameter is a ServiceInfo2 object
    # containing information about the discovered service
    return True


# Populate a subscription filter. Note that all empty fields are considered "wildcards" that match any value
f = RR.ServiceSubscriptionFilter()
# Only connect to services named "reynard"
f.ServiceNames = ['reynard']
# Only use rr+tcp transport
f.TransportSchemes = ['rr+tcp', 'rr+local']
# The ServiceSubscriptionFilter has powerful attribute matching capabilities
# The use is intended for advanced use cases
# f.Attributes = <Advanced attribute matching>
# f.AttributesMatchOperation = ServiceSubscriptionFilterAttributeGroupOperation_AND
# Set a "predicate" function that is called for each discovered service
f.Predicate = subscription_predicate
# Set the maximum number of clients to connect. Default is -1 (no limit)
f.MaxConnections = 5

# Create a ServiceSubscriptionFilterNode for node matching
n = RR.ServiceSubscriptionFilterNode()
# Only connect to nodes named "experimental.reynard_the_robot"
n.NodeName = "experimental.reynard_the_robot"
# Only connect to nodes with NodeID as listed. This will be different for every instance of the node
# n.NodeID = RR.NodeID('12345678-1234-1234-1234-123456789abc')
# Username and credentials can also be set for services that require authentication.
# This should only be used when NodeID is also specified so the credentials are not sent to the wrong node
# n.Username = "username"
# n.Credentials = {"password", RR.VarValue("my_password", "string")}
f.Nodes.append(n)

# Subscribe to a service by type using the filter
root_object_type = "experimental.reynard_the_robot.Reynard"
sub = RRN.SubscribeServiceByType(root_object_type, f)
# Give a few seconds for the service to be connected
time.sleep(6)

# Run a function on all connected services. Could also use GetDefaultClientWait() to
# get the first connected client and run functions on that client
# without waiting for all clients to connect
connected_clients = sub.GetConnectedClients()
for k, c in connected_clients.items():
    c.say("Hello from subscribe_filter!")

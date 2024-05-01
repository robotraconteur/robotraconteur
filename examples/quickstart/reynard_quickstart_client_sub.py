# reynard_quickstart_client_sub.py

from RobotRaconteur.Client import *

# RRN is imported from RobotRaconteur.Client
# Create a subscription and connect to the default client
sub = RRN.SubscribeService('rr+tcp://localhost:53222/?service=reynard')
obj = sub.GetDefaultClientWait(5)

# Call the say function
obj.say("Hello from Reynard!")

# Call the teleport function
obj.teleport(100, 200)

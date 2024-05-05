# reynard_quickstart_client.py

from RobotRaconteur.Client import *

# RRN is imported from RobotRaconteur.Client
# Connect to the service.
obj = RRN.ConnectService('rr+tcp://localhost:53222/?service=reynard')

# Call the say function
obj.say("Hello from Reynard!")

# Call the teleport function
obj.teleport(100, 200)

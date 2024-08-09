# authentication_example.py - Example of using authentication in Python

from RobotRaconteur.Client import *

url = 'rr+tcp://localhost:53226?service=authentication_example'

# Create the user credentials
username = "testuser1"
cred = {"password": RR.VarValue("testpass1", "string")}

# Connect to the service
c = RRN.ConnectService(url, username, cred)

# Use the service
c.say("Message to secure service")

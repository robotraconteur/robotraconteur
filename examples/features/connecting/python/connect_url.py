# connect_url.py - Connect to a service using a URL

from RobotRaconteur.Client import *

url = 'rr+tcp://localhost:29200?service=reynard'
c = RRN.ConnectService(url)
c.say("Hello from connect_url!")

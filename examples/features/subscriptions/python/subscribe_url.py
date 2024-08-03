# subscribe_url.py - Subscribe to a service using a URL

from RobotRaconteur.Client import *

url = 'rr+tcp://localhost:29200?service=reynard'
sub = RRN.SubscribeService(url)
c = sub.GetDefaultClientWait(10)
c.say("Hello from URL subscription!")

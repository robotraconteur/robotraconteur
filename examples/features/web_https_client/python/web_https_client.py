# web_https_client.py - Example of connecting to a standard
# web server using HTTPS and WebSockets

from RobotRaconteur.Client import *

# Connect to the service using rr+wss, the secure WebSocket transport
url = 'rr+wss://wstest2.wasontech.com/robotraconteur?service=testobj'
c = RRN.ConnectService(url)

assert c.add_two_numbers(2, 4) == 6

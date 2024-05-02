# reynard_quickstart_service.py

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s

import threading
import reynard_the_robot

# Define the service definition for the quickstart service
reynard_quickstart_interface = """
service experimental.reynard_quickstart

object ReynardQuickstart
    function void say(string text)
    function void teleport(double x, double y)
end
"""

# Implement the quickstart service


class ReynardQuickstartImpl(object):
    def __init__(self):
        self.reynard = reynard_the_robot.Reynard()
        self.reynard.start()
        self._lock = threading.Lock()

    def say(self, text):
        with self._lock:
            self.reynard.say(text)

    def teleport(self, x, y):
        with self._lock:
            self.reynard.teleport(x, y)


with RR.ServerNodeSetup("experimental.reynard_quickstart", 53222):
    # Register the service type
    RRN.RegisterServiceType(reynard_quickstart_interface)

    reynard_inst = ReynardQuickstartImpl()

    # Register the service
    RRN.RegisterService("reynard", "experimental.reynard_quickstart.ReynardQuickstart", reynard_inst)

    # Wait for program exit to quit
    input("Press enter to quit")

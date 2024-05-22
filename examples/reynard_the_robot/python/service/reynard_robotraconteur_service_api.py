# Python example Robot Raconteur service for Reynard the Robot using the Python API to communicate with the robot.
# For this example, the Reynard Robot runs inside the same process as the service. This example
# is representative of a device that provides a software library for commanding the device. The API
# can be provided in different programming languages, but for this example, the API is in Python.

# Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
# to use meters and radians. The service will convert the units as needed.

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import threading
import numpy as np
import drekar_launch_process
import reynard_the_robot
from pathlib import Path

# Define the class that implements the service. In Python, the thunk code is dynamically handled
# so there is no need to generate the service definition.


class Reynard_impl:

    def __init__(self, reynard, node=None):
        if node is None:
            self._node = RR.RobotRaconteurNode.s
        else:
            self._node = node

        self._reynard = reynard
        self._lock = threading.Lock()

        self._reynard_state_type = self._node.GetStructureType("experimental.reynard_the_robot.ReynardState")

        self.new_message = RR.EventHook()

        reynard.new_message.connect(self._new_message)

        self._state_timer = None

    def _start(self):
        self._state_timer = self._node.CreateTimer(0.05, self._timer_cb, False)
        self._state_timer.Start()

    def _stop(self):
        if self._state_timer is not None:
            self._state_timer.TryStop()
            self._state_timer = None

    def _new_message(self, _, message):
        self.new_message.fire(message)

    def teleport(self, x, y):
        with self._lock:
            if x > 1 or x < -1 or y > 0.5 or y < -0.5:
                raise RR.InvalidArgumentException("Teleport target position is out of range")
            # Convert from m to mm
            x1 = x * 1e3
            y1 = y * 1e3
            self._reynard.teleport(x1, y1)

    def setf_arm_position(self, q1, q2, q3):
        with self._lock:
            # Convert from radians to degrees
            q1_1 = np.rad2deg(q1)
            q2_1 = np.rad2deg(q2)
            q3_1 = np.rad2deg(q3)
            self._reynard.set_arm_position(q1_1, q2_1, q3_1)

    def getf_arm_position(self):
        return np.deg2rad(self._reynard.arm_position)

    @property
    def robot_position(self):
        return np.array(self._reynard.robot_position, dtype=np.float64) * 1e-3

    def drive_robot(self, vel_x, vel_y, timeout, wait):
        vel_x_1 = vel_x * 1e3
        vel_y_1 = vel_y * 1e3
        self._reynard.drive_robot(vel_x_1, vel_y_1, timeout, wait)

    def drive_arm(self, q1, q2, q3, timeout, wait):
        q1_1 = np.rad2deg(q1)
        q2_1 = np.rad2deg(q2)
        q3_1 = np.rad2deg(q3)

        self._reynard.drive_arm(q1_1, q2_1, q3_1, timeout, wait)

    def say(self, message):
        self._reynard.say(message)

    @property
    def color(self):
        with self._lock:
            return self._reynard.color

    @color.setter
    def color(self, c):
        if len(c) != 3:
            raise RR.InvalidArgumentException("Expected an array with length of 3")
        if np.any(np.array(c) < 0) or np.any(np.array(c) > 1):
            raise RR.InvalidArgumentException("Invalid color value")
        self._reynard.color = c

    def _timer_cb(self, evt):
        s = self._reynard_state_type()
        s.time = self._reynard.time
        s.robot_position = np.array(self._reynard.robot_position, dtype=np.float64)
        s.arm_position = np.array(self._reynard.arm_position, dtype=np.float64)
        s.robot_velocity = np.array(self._reynard.robot_velocity, dtype=np.float64)
        s.arm_velocity = np.array(self._reynard.arm_velocity, dtype=np.float64)

        self.state.OutValue = s


def main():

    # Create a Reynard instance
    reynard = reynard_the_robot.Reynard()
    reynard.start()

    # Use the robdef from a file. In practice, this is usually done using
    # a package resource. See RobotRaconteurCompanion.Util.RobDef.register_service_types_from_resources
    RRN.RegisterServiceTypesFromFiles(
        [str(Path(__file__).parent.parent.parent / "robdef" / "experimental.reynard_the_robot.robdef")])

    # Create the Reynard service instance
    reynard_obj = Reynard_impl(reynard)

    # Use RobotRaconteur.ServerNodeSetup to initialize Robot Raconteur using the default node
    with RR.ServerNodeSetup("experimental.reynard_the_robot_python_rest", 59201):

        # Register the service with Robot Raconteur
        ctx = RRN.RegisterService("reynard", "experimental.reynard_the_robot.Reynard", reynard_obj)

        # Start the service timer for updating the state
        reynard_obj._start()

        # Print some info for the user
        print("Reynard the Robot Python Service Started")
        print()
        print("Candidate connection urls:")
        ctx.PrintCandidateConnectionURLs()
        print()
        print("Press Ctrl-C to quit...")

        # Use drekar_launch_process package to wait for exit
        drekar_launch_process.wait_exit()

        # Stop the service timer
        reynard_obj._stop()

    reynard.stop()


if __name__ == '__main__':
    main()

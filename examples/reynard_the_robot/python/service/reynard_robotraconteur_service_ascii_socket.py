# Python example Robot Raconteur service for Reynard the Robot using the ASCII Socket API to communicate with the robot.

# Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
# to use meters and radians. The service will convert the units as needed.

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np
from pathlib import Path
import socket
import drekar_launch_process
import shlex

# Define the class that implements the service. In Python, the thunk code is dynamically handled
# so there is no need to generate the service definition.

# This example uses a simple ASCII socket interface to communicate with the robot. Many industrial devices
# will use an ASCII socket interface or serial port interface. Many other devices will use a binary interface.
# Binary interfaces are more efficient but are also more complicated. See the iRobot Create example for a
# binary serial port interface example. Many devices that use a binary socket interface will provide a client
# library that implements the protocol.


class Reynard_impl:
    def __init__(self, host="localhost", port=29202):
        self._host = host
        self._port = port
        self._state_timer = None

        # Retrieve the structure type for ReynardState
        self._state_type = RRN.GetStructureType("experimental.reynard_the_robot.ReynardState")

        # Create the EventHook for new_message event
        self.new_message = RR.EventHook()

        # Wires are automatically created when the service is registered when [readonly] or [writeonly] is used

    def _start(self):
        self._state_timer = RRN.CreateTimer(0.1, self._timer_cb)
        self._state_timer.Start()

    def _stop(self):
        if self._state_timer is not None:
            self._state_timer.TryStop()
            self._state_timer = None

    def _communicate(self, text_request):
        with socket.create_connection(("localhost", 29202)) as s:
            s.settimeout(5)

            f = s.makefile("rw")
            f.writelines([f"{text_request}\n"])
            f.flush()
            state_str = f.readline().strip()
            state_str_p = shlex.split(state_str)
            if state_str_p[0] == "ERROR":
                raise RR.OperationFailedException(state_str_p[1])
            state_str_p.pop(0)
            return state_str_p

    @property
    def robot_position(self):
        st = self._communicate("STATE")
        return [float(st[1]) / 1000.0, float(st[2]) / 1000.0]

    @property
    def color(self):
        st = self._communicate("COLORGET")
        return [float(st[0]), float(st[1]), float(st[2])]

    @color.setter
    def color(self, value):
        self._communicate(f"COLORSET {value[0]} {value[1]} {value[2]}")

    def teleport(self, x, y):
        self._communicate(f"TELEPORT {int(x * 1000)} {int(y * 1000)}")

    def setf_arm_position(self, q1, q2, q3):
        self._communicate(f"SETARM {np.rad2deg(q1)} {np.rad2deg(q2)} {np.rad2deg(q3)}")

    def getf_arm_position(self):
        st = self._communicate("STATE")
        return [np.deg2rad(float(st[3])), np.deg2rad(float(st[4])), np.deg2rad(float(st[5]))]

    def drive_robot(self, vel_x, vel_y, timeout=-1, wait=False):
        self._communicate(f"DRIVE {int(vel_x * 1000)} {int(vel_y * 1000)} {timeout} {int(wait)}")

    def drive_arm(self, q1, q2, q3, timeout=-1, wait=False):
        self._communicate(f"DRIVEARM {np.rad2deg(q1)} {np.rad2deg(q2)} {np.rad2deg(q3)} {timeout} {int(wait)}")

    def say(self, message):
        self._communicate(f"SAY \"{message}\"")

    def _timer_cb(self, ev):
        try:
            st = self._communicate("STATE")
            state = self._state_type()
            state.time = float(st[0])
            state.robot_position = 1e-3 * np.array([float(st[1]), float(st[2])], dtype=np.float64)
            state.arm_position = np.deg2rad(np.array([float(st[3]), float(st[4]), float(st[5])], dtype=np.float64))
            state.robot_velocity = []
            state.arm_velocity = []

            if self.state is not None:
                self.state.OutValue = state

        except Exception as e:
            RRN.LogMessage(RR.LogLevel_Warning, "Error in timer callback: " + str(e))


def main():

    # Set the host and port for the ascii socket interface
    host = "localhost"
    port = 29202

    # Use the robdef from a file. In practice, this is usually done using
    # a package resource. See RobotRaconteurCompanion.Util.RobDef.register_service_types_from_resources
    RRN.RegisterServiceTypesFromFiles(
        [str(Path(__file__).parent.parent.parent / "robdef" / "experimental.reynard_the_robot.robdef")])

    # Create the Reynard service instance
    reynard_obj = Reynard_impl(host, port)

    # Use RobotRaconteur.ServerNodeSetup to initialize Robot Raconteur using the default node
    with RR.ServerNodeSetup("experimental.reynard_the_robot_ascii_socket", 59201):

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


if __name__ == '__main__':
    main()

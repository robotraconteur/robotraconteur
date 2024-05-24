# Python example Robot Raconteur service for Reynard the Robot using the HTTP REST API to communicate with the robot.

# Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
# to use meters and radians. The service will convert the units as needed.

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np
from pathlib import Path
import requests
import drekar_launch_process

# Define the class that implements the service. In Python, the thunk code is dynamically handled
# so there is no need to generate the service definition.


class Reynard_impl:
    def __init__(self, base_url="http://localhost:29201"):
        self._base_url = base_url
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

    @property
    def robot_position(self):
        res = requests.get(self._base_url + "/api/state")
        res.raise_for_status()
        res_json = res.json()
        return [res_json["x"] / 1000.0, res_json["y"] / 1000.0]

    @property
    def color(self):
        res = requests.get(self._base_url + "/api/color")
        res.raise_for_status()
        res_json = res.json()
        return [res_json["r"], res_json["g"], res_json["b"]]

    @color.setter
    def color(self, value):
        res = requests.post(self._base_url + "/api/color", json={"r": value[0], "g": value[1], "b": value[2]})
        res.raise_for_status()

    def teleport(self, x, y):
        res = requests.post(self._base_url + "/api/teleport", json={"x": x * 1000, "y": y * 1000})
        res.raise_for_status()

    def setf_arm_position(self, q1, q2, q3):
        res = requests.post(self._base_url + "/api/arm",
                            json={"q1": np.rad2deg(q1), "q2": np.rad2deg(q2), "q3": np.rad2deg(q3)})
        res.raise_for_status()

    def getf_arm_position(self):
        res = requests.get(self._base_url + "/api/arm")
        res.raise_for_status()
        res_json = res.json()
        return [np.deg2rad(res_json["q1"]), np.deg2rad(res_json["q2"]), np.deg2rad(res_json["q3"])]

    def drive_robot(self, vel_x, vel_y, timeout=-1, wait=False):
        res = requests.post(self._base_url + "/api/drive_robot",
                            json={"vel_x": vel_x * 1000.0, "vel_y": vel_y * 1000.0, "timeout": timeout, "wait": wait})
        res.raise_for_status()

    def drive_arm(self, q1, q2, q3, timeout=-1, wait=False):
        res = requests.post(self._base_url + "/api/drive_arm",
                            json={"q1": np.rad2deg(q1), "q2": np.rad2deg(q2), "q3": np.rad2deg(q3), "timeout": timeout, "wait": wait})
        res.raise_for_status()

    def say(self, message):
        res = requests.post(self._base_url + "/api/say", json={"message": message})
        res.raise_for_status()

    def _timer_cb(self, ev):
        try:
            res = requests.get(self._base_url + "/api/state")
            res.raise_for_status()
            res_json = res.json()
            state = self._state_type()
            state.time = res_json["time"]
            state.robot_position = 1e-3 * np.array([res_json["x"], res_json["y"]], dtype=np.float64)
            state.arm_position = np.deg2rad(
                np.array([res_json["q1"], res_json["q2"], res_json["q3"]], dtype=np.float64))
            state.robot_velocity = 1e-3 * np.array([res_json["vel_x"], res_json["vel_y"]], dtype=np.float64)
            state.arm_velocity = np.deg2rad(
                np.array([res_json["vel_q1"], res_json["vel_q2"], res_json["vel_q3"]], dtype=np.float64))

            if self.state is not None:
                self.state.OutValue = state

            message_res = requests.get(self._base_url + "/api/messages")
            message_res.raise_for_status()
            message_res_json = message_res.json()
            for message in message_res_json:
                self.new_message.fire(message)

        except Exception as e:
            RRN.LogMessage(RR.LogLevel_Warning, "Error in timer callback: " + str(e))


def main():

    # Set the base URL for the Reynard the Robot HTTP REST API
    base_url = "http://localhost:29201"

    # Use the robdef from a file. In practice, this is usually done using
    # a package resource. See RobotRaconteurCompanion.Util.RobDef.register_service_types_from_resources
    RRN.RegisterServiceTypesFromFiles(
        [str(Path(__file__).parent.parent.parent / "robdef" / "experimental.reynard_the_robot.robdef")])

    # Create the Reynard service instance
    reynard_obj = Reynard_impl(base_url)

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


if __name__ == '__main__':
    main()

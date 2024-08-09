##
# Command the ABB IRB 1200 robot using the position_command wire
#

from RobotRaconteur.Client import *
import time
import numpy as np
import sys

# URL for connecting to the robot. By default connects to UR5e simulated robot.
url = 'rr+tcp://localhost:52511?service=robot'
if (len(sys.argv) >= 2):
    url = sys.argv[1]

# Connect to the robot driver
c = RRN.ConnectService(url)

# Retrieve the current robot state and print the current command mode
print(c.robot_state.PeekInValue()[0].command_mode)

# Retrieve the constants for the com.robotraconteur.robotics.robot service definition
robot_const = RRN.GetConstants("com.robotraconteur.robotics.robot", c)

# Retrieve the "halt" and "position_command" enum values
halt_mode = robot_const["RobotCommandMode"]["halt"]
position_mode = robot_const["RobotCommandMode"]["position_command"]

# Retreive the structure type to create RobotJointCommand objects
RobotJointCommand = RRN.GetStructureType("com.robotraconteur.robotics.robot.RobotJointCommand", c)

# Change the robot command mode, first to halt, then to position_mode
c.command_mode = halt_mode
time.sleep(0.1)
c.command_mode = position_mode
time.sleep(0.1)

# Connect to the position_command and robot_state wires for real-time data streaming
cmd_w = c.position_command.Connect()
state_w = c.robot_state.Connect()
# Set a lifespan of 0.5 seconds for the robot state. If new packets
# are not received within 0.5 seconds, an exception will be thrown
state_w.InValueLifespan = 0.5

# Wait for the state_w wire to receive valid data
state_w.WaitInValueValid()

# Initialize command_seqno. command_seqno must increment for each position command
command_seqno = 1

# Retreive the current joint position
init_joint_pos = state_w.InValue.joint_position

t0 = time.time()

while (True):
    t = time.time() - t0

    # Retreive the current robot state
    robot_state = state_w.InValue

    # Make sure the robot is still in position mode
    if (robot_state.command_mode != position_mode):
        raise Exception("Robot is not in position mode")

    # Increment command_seqno
    command_seqno += 1

    # Create Fill the RobotJointCommand structure
    joint_cmd1 = RobotJointCommand()
    joint_cmd1.seqno = command_seqno  # Strictly increasing command_seqno
    joint_cmd1.state_seqno = robot_state.seqno  # Send current robot_state.seqno as failsafe

    # Generate a joint command, in this case a sin wave
    cmd = 0.2 * np.array([1, 0, 0, 0, 0, 0]) * np.sin(t) + init_joint_pos
    # Set the joint command
    joint_cmd1.command = cmd

    # Send the joint command to the robot
    cmd_w.OutValue = joint_cmd1

    # Wait for next iteration
    time.sleep(.01)

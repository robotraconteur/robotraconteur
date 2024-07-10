##
# Command the robot using the jog_freespace function
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

# Retrieve the "halt" and "jog" enum values
halt_mode = robot_const["RobotCommandMode"]["halt"]
jog_mode = robot_const["RobotCommandMode"]["jog"]

# Change the robot command mode, first to halt, then to jog
c.command_mode = halt_mode
time.sleep(0.1)
c.command_mode = jog_mode


# Get the starting joint positions
robot_state = c.robot_state.PeekInValue()[0]
start_joint_pos = robot_state.joint_position

t_start = time.time()

# Move the robot in an endless loop
while (True):
    t = time.time() - t_start

    # Generate a sin wave
    c.jog_freespace(0.2 * np.array([1, 0, 0, 0, 0, 0]) * np.sin(t / 5) + start_joint_pos, np.ones((6,)), True)

    # Print the current robot_state_flags
    print(hex(c.robot_state.PeekInValue()[0].robot_state_flags))

    # Wait for the next loop iteration
    time.sleep(0.1)

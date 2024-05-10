from RobotRaconteur.Client import *
import time
import numpy as np

# Connect to the Reynard service using a URL
c = RRN.ConnectService('rr+tcp://localhost:29200?service=reynard')


def new_message(msg):
    print(f"New message: {msg}")


# Connect a callback function to listen for new messages
c.new_message += new_message

# Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
state, _ = c.state.PeekInValue()
print(state)

# Teleport the robot
c.teleport(0.1, -0.2)

# Drive the robot with no timeout
c.drive_robot(0.5, -0.2, -1, False)

# Wait for one second
time.sleep(1)

# Stop the robot
c.drive_robot(0, 0, -1, False)

# Set the arm position
c.setf_arm_position(np.deg2rad(100), np.deg2rad(-30), np.deg2rad(-70))

# Drive the arm using timeout and wait
c.drive_arm(np.deg2rad(10), np.deg2rad(-30), np.deg2rad(-15), 1.5, True)

# Set the color to red
c.color = (1, 0, 0)

# Read the color
print(f"Color: {c.color}")

time.sleep(1)

# Reset the color
c.color = (0.929, 0.49, 0.192)

# Say hello
c.say("Hello, World From Robot Raconteur!")

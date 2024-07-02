##
# Command the first ABB IRB 1200 robot using the jog_freespace function
#

from RobotRaconteur.Client import *
import time
import numpy as np
import sys

# URL for connecting to the robot. By default connects to UR5e simulated robot.
url = 'rr+tcp://localhost:52511?service=robot'
if (len(sys.argv) >= 3):
    url = sys.argv[2]

# Connect to the robot
c = RRN.ConnectService(url)

# Get the robot_info data from the driver
robot_info = c.robot_info

# Get the joint names from the robot_info data structure
joint_names = [j.joint_identifier.name for j in robot_info.joint_info]

# Retrieve the current robot state and print the current command mode
print(c.robot_state.PeekInValue()[0].command_mode)

# Retrieve the constants for the com.robotraconteur.robotics.robot service definition
robot_const = RRN.GetConstants("com.robotraconteur.robotics.robot", c)

# Retrieve the "halt" and "trajectory" enum values
halt_mode = robot_const["RobotCommandMode"]["halt"]
trajectory_mode = robot_const["RobotCommandMode"]["trajectory"]

# Retreive the structure type to create JointTrajectory and JointTrajectoryWaypoint objects
JointTrajectoryWaypoint = RRN.GetStructureType("com.robotraconteur.robotics.trajectory.JointTrajectoryWaypoint", c)
JointTrajectory = RRN.GetStructureType("com.robotraconteur.robotics.trajectory.JointTrajectory", c)

# Change the robot command mode, first to halt, then to trajectory mode
c.command_mode = halt_mode
time.sleep(0.1)
c.command_mode = trajectory_mode

# Connect to the robot_state wire to get real-time streaming state data
state_w = c.robot_state.Connect()

# Wait for the state_w wire to receive valid data
state_w.WaitInValueValid()
state1 = state_w.InValue


# Build up JointTrajectoryWaypoint(s) to move the robot to specified joint angles
waypoints = []

j_start = state1.joint_position
if sys.argv[1] == 'abb':
    j_end = [0, -0.1, 0.25, 0, 0, 0]
elif sys.argv[1] == 'ur':
    j_end = [0, -1, -1.5, 0, 0, 0]
else:
    raise ValueError('Invalid robot type')

for i in range(251):
    wp = JointTrajectoryWaypoint()
    wp.joint_position = (j_end - j_start) * (float(i) / 250.0) + j_start
    wp.time_from_start = i / 25.0
    waypoints.append(wp)

# Fill in the JointTrajectory structure
traj = JointTrajectory()
traj.joint_names = joint_names
traj.waypoints = waypoints

c.speed_ratio = 1

# Execute the trajectory function to get the generator object
traj_gen = c.execute_trajectory(traj)

# Loop to monitor the motion
while (True):
    t = time.time()

    # Check the state
    robot_state = state_w.InValue

    res, gen_state = traj_gen.TryNext()
    if not res:
        # Break if the trajectory is complete
        break
    print(gen_state)

    print(hex(c.robot_state.PeekInValue()[0].robot_state_flags))

# Execute more trajectories. Trajectories are queued if executed concurrently

waypoints = []

for i in range(251):
    t = float(i) / 25.0
    wp = JointTrajectoryWaypoint()
    cmd = np.deg2rad(15) * np.sin(2 * np.pi * (t / 10.0)) * np.array([1, 0, 0, 0, 0.5, -1])
    cmd = cmd + j_end
    wp.joint_position = cmd
    wp.time_from_start = t
    waypoints.append(wp)

traj = JointTrajectory()
traj.joint_names = joint_names
traj.waypoints = waypoints

c.speed_ratio = 0.5

traj_gen = c.execute_trajectory(traj)

c.speed_ratio = 2
traj_gen2 = c.execute_trajectory(traj)

res = traj_gen2.Next()
print(res.action_status)

while (True):
    t = time.time()

    robot_state = state_w.InValue

    res, gen_state = traj_gen.TryNext()
    if not res:
        # Break if the trajectory is complete
        break
    print(gen_state.action_status)

while (True):
    t = time.time()

    robot_state = state_w.InValue

    res, gen_state = traj_gen2.TryNext()
    if not res:
        # Break if the trajectory is complete
        break
    print(gen_state.action_status)

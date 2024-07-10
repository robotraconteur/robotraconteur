# Robot Matlab Client Examples

This directory contains examples for controlling the standard robots robot using the standard driver service.
Use with either the [robotraconteur_training_sim](https://github.com/robotraconteur-contrib/robotraconteur_training_sim)
with the "multi-robot scene", or a real ABB IRB 1200 robot or Universal Robots UR5e
configured with the [ABBRobotRaconteurDriver](https://github.com/robotraconteur-contrib/ABBRobotRaconteurDriver) or
[URRobotRaconteurDriver](https://github.com/robotraconteur-contrib/URRobotRaconteurDriver).
By default the examples will connect to the URL for the training
simulator UR5e robot. Change the URL of the real robot driver or different robot in the training simulator.
The real URL can be found using the
[Robot Raconteur Service Browser](https://github.com/robotraconteur/RobotRaconteur_ServiceBrowser).

Some examples require specifying if the ABB or UR5e robot is being used because of the joint configuration.
Check the example for details.

See the [Reynard the Robot client example readme](../../../reynard_the_robot/matlab/client/README.md)
for instructions to set up the Matlab environment.

Examples:

- `robot_client_jog_freespace.m` - Use "jog" mode to move the robot
- `robot_client_position_command.m` - Use streaming "position command" to directly control the robot in real-time
- `robot_client_trajectory.m` - Send a trajectory to the robot to be executed in "trajectory mode". Change the robot_type
  variable to `abb` or `ur` to specify which robot is being used.

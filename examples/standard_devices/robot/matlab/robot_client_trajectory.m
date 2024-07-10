% Example controlling standard robot using trajectory mode

robot_type = 'ur'; % Change to abb for ABB robot joint position

% URL for connecting to the robot. By default connects to UR5e simulated robot.
url = 'rr+tcp://localhost:52511?service=robot';

% Connect to the robot driver
c = RobotRaconteur.ConnectService(url);

% Get the robot_info data from the driver
robot_info = c.robot_info;

% Get the joint names from the robot_info data structure
joint_names = {};
for i=1:size(robot_info.joint_info)
    joint_names{end+1,1} = robot_info.joint_info{i}.joint_identifier.name;
end

% Retrieve the current robot state and print the current command mode
robot_state = c.robot_state.PeekInValue();
disp(robot_state.command_mode);

% Change the robot command mode, first to halt, then to jog
c.command_mode = int32(0);
pause(0.1)
c.command_mode = int32(2);

% Get the starting joint positions
robot_state = c.robot_state.PeekInValue();
j_start = robot_state.joint_position;

% Build up JointTrajectoryWaypoint(s) to move the robot to specified joint angles
waypoints = {};

if strcmp(robot_type, 'abb')
    j_end = [0, -0.1, 0.25, 0, 0, 0]';
elseif strcmp(robot_type, 'ur')
    j_end = [0, -1, -1.5, 0, 0, 0]';
else
    error('Invalid robot type');
end

for i=0:250
    wp=struct;
    wp.joint_position = (j_end - j_start) * (i / 250.0) + j_start;
    wp.joint_velocity = [];
    wp.position_tolerance = [];
    wp.velocity_tolerance = [];
    wp.interpolation_mode = int32(0);
    wp.waypoint_type = int32(0);
    wp.time_from_start = i / 25.0;
    waypoints{end+1,1} = wp;
end

% Fill in the JointTrajectory structure
traj=RobotRaconteur.CreateStructure(c, 'com.robotraconteur.robotics.trajectory.JointTrajectory');
traj.joint_names = joint_names;
traj.waypoints = waypoints;

c.speed_ratio = 1.0;

% Execute the trajectory function to get the generator object
traj_gen = c.execute_trajectory(traj);

% Loop to monitor the motion

while true

    try_next_res = traj_gen.TryNext();
    res = try_next_res{1};
    trajectory_status = try_next_res{2};
    if ~res
        break
    end

    robot_state = c.robot_state.PeekInValue();
    disp(robot_state.robot_state_flags);
end

% Execute more trajectories. Trajectories are queued if executed concurrently

waypoints2 = {};

for i=0:250

    t = i/25;
    wp=struct;
    cmd = deg2rad(15) * sin(2 * pi * (t/10.0)) * [1, 0, 0, 0, 0.5, -1]';
    cmd = cmd + j_end;
    wp.joint_position = cmd;
    wp.joint_velocity = [];
    wp.position_tolerance = [];
    wp.velocity_tolerance = [];
    wp.interpolation_mode = int32(0);
    wp.waypoint_type = int32(0);
    wp.time_from_start = i / 25.0;
    waypoints2{end+1,1} = wp;
end

traj2=RobotRaconteur.CreateStructure(c, 'com.robotraconteur.robotics.trajectory.JointTrajectory');
traj2.joint_names = joint_names;
traj2.waypoints = waypoints2;

c.speed_ratio = 0.5;

traj_gen2 = c.execute_trajectory(traj2);
traj_gen2.TryNext();

c.speed_ratio = 2.0;

traj_gen3 = c.execute_trajectory(traj2);
traj_gen3.TryNext();


while true

    try_next_res = traj_gen2.TryNext();
    res = try_next_res{1};
    trajectory_status = try_next_res{2};
    if ~res
        break
    end

    robot_state = c.robot_state.PeekInValue();
    disp(robot_state.robot_state_flags);
end

while true

    try_next_res = traj_gen3.TryNext();
    res = try_next_res{1};
    trajectory_status = try_next_res{2};
    if ~res
        break
    end

    robot_state = c.robot_state.PeekInValue();
    disp(robot_state.robot_state_flags);
end

RobotRaconteur.DisconnectService(c);

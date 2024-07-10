% Example controlling standard robot using jog mode

% URL for connecting to the robot. By default connects to UR5e simulated robot.
url = 'rr+tcp://localhost:52511?service=robot';

% Connect to the robot driver
c = RobotRaconteur.ConnectService(url);

% Retrieve the current robot state and print the current command mode
robot_state = c.robot_state.PeekInValue();
disp(robot_state.command_mode);

% Change the robot command mode, first to halt, then to jog
c.command_mode = int32(0);
pause(0.1)
c.command_mode = int32(1);

% Get the starting joint positions
robot_state = c.robot_state.PeekInValue();
start_joint_pos = robot_state.joint_position;

start_time = tic;

% Move the robot in a loop
for i=1:100
    t = toc(start_time);

    % Generate a sin wave
    c.jog_freespace(0.2 * [1, 0, 0, 0, 0, 0]' * sin(t / 5) + start_joint_pos, ones(6,1), true);

    % Print the current robot_state_flags
    robot_state = c.robot_state.PeekInValue();
    disp(robot_state.robot_state_flags);

    % Wait for the next loop iteration
    pause(0.1);
end

% Disconnect the client. Client connections will be closed on Matlab exit
% but will not be automatically closed otherwise.
RobotRaconteur.DisconnectService(c);

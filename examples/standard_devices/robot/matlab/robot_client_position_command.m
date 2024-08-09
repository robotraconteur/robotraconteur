% Example controlling standard robot using streaming position command mode

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
c.command_mode = int32(3);
pause(0.1)

% Connect to the position_command and robot_state wires for real-time data streaming
cmd_w = c.position_command.Connect();
state_w = c.robot_state.Connect();
% Set a lifespan of 0.5 seconds for the robot state. If new packets
% are not received within 0.5 seconds, an exception will be thrown
state_w.InValueLifespan = 0.5;

% Wait for the state_w wire to receive valid data
state_w.WaitInValueValid();

% Initialize command_seqno. command_seqno must increment for each position command
command_seqno = uint64(1);

% Retrieve the current joint position
robot_state = state_w.InValue;
init_joint_pos = robot_state.joint_position;

start_time = tic;

% Move the robot in a loop
for i=1:1000
    t = toc(start_time);

    % Retrieve the current robot state
    robot_state = state_w.InValue;

    % Make sure the robot is still in position mode
    if robot_state.command_mode ~= int32(3)
        error("Robot is not in position mode")
    end

    % Increment command_seqno
    command_seqno = command_seqno + 1;

    % Create Fill the RobotJointCommand structure
    joint_cmd1 = RobotRaconteur.CreateStructure(c, 'com.robotraconteur.robotics.robot.RobotJointCommand');
    joint_cmd1.seqno = command_seqno;  % Strictly increasing command_seqno
    joint_cmd1.state_seqno = robot_state.seqno;  % Send current robot_state.seqno as failsafe

    % Generate a joint command, in this case a sin wave
    cmd = 0.2 * [1, 0, 0, 0, 0, 0]' * sin(t) + init_joint_pos;
    % Set the joint command
    joint_cmd1.command = cmd;

    % Send the joint command to the robot
    cmd_w.OutValue = joint_cmd1;

    % Wait for the next loop iteration
    pause(0.01);
end

% Disconnect the client. Client connections will be closed on Matlab exit
% but will not be automatically closed otherwise.
RobotRaconteur.DisconnectService(c);

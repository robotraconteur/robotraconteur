function reynard_robotraconteur_client()

% Connect to the Reynard service using a URL
c = RobotRaconteur.ConnectService('rr+tcp://localhost:29200?service=reynard');

% Enable events. Don't use if no event listeners are connected
RobotRaconteur.EnableEvents(c);

% Connect a callback function to listen for new messages
evt = addlistener(c, 'new_message', @new_message_callback);

% Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
state = c.state.PeekInValue();
disp(state);

% Teleport the robot
c.teleport(0.1,-0.2);

% Drive the robot with no timeout
c.drive_robot(0.5,-0.2,-1,false);

% Wait for one second
pause(1)

% Stop the robot
c.drive_robot(0,0,-1,false);

% Set the arm position
c.setf_arm_position(deg2rad(100), deg2rad(-30), deg2rad(-70));

% Drive the arm using timeout and wait
c.drive_arm(deg2rad(10), deg2rad(-30), deg2rad(-15), 1.5, true);

% Set the color to red
c.color = [1,0,0]';

% Read the color
c_color = c.color;
disp(c_color)

pause(1);

% Reset the color
c.color = [0.929, 0.49, 0.192]';

% Say hello
c.say('Hello, World From Matlab!');

% Process events using ProcessRequests() function.
% Matlab is single threaded, so asynchronous events need to
% be executed using ProcessRequests()
for i=1:10
    RobotRaconteur.ProcessRequests();
    pause(0.1);
end

    % Callback function for new_message
    function new_message_callback(msg)
       disp(['New message: ' msg]);
    end

end

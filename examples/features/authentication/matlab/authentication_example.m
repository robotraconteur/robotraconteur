% authentication_example.m - Example of using authentication from Matlab

url = 'rr+tcp://localhost:53226?service=authentication_example';

% Create the user credentials
username = "testuser1";
cred = containers.Map({'password'}, {RobotRaconteurVarValue('testpass1', 'string')});

% Connect to the service
c = RobotRaconteur.ConnectService(url, username, cred);

% Use the service
c.say('Message to secure service');

RobotRaconteur.DisconnectService(c);

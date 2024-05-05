% reynard_quickstart_client.m

% Connect to the service
o = RobotRaconteur.ConnectService('rr+tcp://localhost:53222/?service=reynard');

% Call the say function
o.say("Hello from MATLAB!");

% Call the teleport function
o.teleport(-150,200);

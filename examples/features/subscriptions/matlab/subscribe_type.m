% subscribe_type.py - Subscribe to services by type

root_object_type = 'experimental.reynard_the_robot.Reynard';
sub = RobotRaconteur.SubscribeServiceByType(root_object_type);

% Give a few seconds for the service to be connected
pause(6);

% Run a function on all connected services. Could also use
% GetDefaultClientWait() to get the first connected client and
% run functions on that client without waiting for all clients to connect
connected_clients = sub.GetConnectedClients();

% Loop connected clients and call say method
client_keys = keys(connected_clients);
for i = 1:length(client_keys)
    key = client_keys{i};
    c = connected_clients(key);
    c.say('Hello from type subscription!');
end

sub.Close();

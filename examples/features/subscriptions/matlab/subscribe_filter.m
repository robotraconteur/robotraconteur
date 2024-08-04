% subscribe_filter.py - Subscribe to services by type using a filter

function subscribe_filter()

% Populate a subscription filter. Note that all missing fields are
% considered "wildcards" that match any value
f = struct;
% Only connect to services named "reynard"
f.ServiceNames = {'reynard'};
% Only use rr+tcp transport
f.TransportSchemes = {'rr+tcp'};
% The ServiceSubscriptionFilter has powerful attribute matching capabilities
% The use is intended for advanced use cases
% f.Attributes = <Advanced attribute matching>
% f.AttributesMatchOperation = 'and';
% "Predicate" not supported in matlab
% Set the maximum number of clients to connect. Default is -1 (no limit)
f.MaxConnections = -1;

% Create a ServiceSubscriptionFilterNode for node matching
n = struct;
% Only connect to nodes named "experimental.reynard_the_robot"
n.NodeName = 'experimental.reynard_the_robot';
% Only connect to nodes with NodeID as listed. This will be different for every instance of the node
% n.NodeID = '12345678-1234-1234-1234-123456789abc';
%Username and credentials can also be set for services that require authentication.
%This should only be used when NodeID is also specified so the credentials are not sent to the wrong node
%n.Username = 'username';
%n.Credentials = containers.Map({'password'}, {RobotRaconteurVarValue('my_password', 'string')});
f.Nodes = {n};

% Subscribe to a service by type using the filter
root_object_type = 'experimental.reynard_the_robot.Reynard';
sub = RobotRaconteur.SubscribeServiceByType(root_object_type, f);

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
    c.say('Hello from subscribe_filter!');
end

sub.Close();

end

% null_value_types.m - Example of types that can be null

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Null values
% Null values are "missing" in Matlab
% struct, containers, and varvalue are nullable.
% Numbers, arrays, strings, pods, and namedarrays are not nullable.

% property MyStructure t_struct_null
c.t_struct_null = missing;
assert(ismissing(c.t_struct_null));

RobotRaconteur.DisconnectService(c);

% struct_value_type.m - Simple example of using struct value types

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Create and populate a MyStructure
s = RobotRaconteur.CreateStructure(c, 'experimental.value_types.MyStructure');
% field double a
s.a = 5;
% field uint32[2] b
s.b = uint32([10, 20]');
% field string c
s.c = 'String from structure client';
% field string{list} d
s.d = {'string a','string b'}';


% Set the property using the structure
% property MyStructure k_struct
c.k_struct = s;

% Retrieve the structure from the service
u = c.k_struct;

% Structures can be None
assert(~ismissing(u));

% field double a
assert (u.a == 52);
% field uint32[2] b
assert(isequal(u.b, [110, 120]'));
% field string c
assert (strcmp(u.c, 'String from structure service'));
% field string{list} d
assert (length(u.d) == 3);
assert (strcmp(u.d{1}, 'string c'));
assert (strcmp(u.d{2}, 'string d'));
assert (strcmp(u.d{3}, 'string e'));

RobotRaconteur.DisconnectService(c);

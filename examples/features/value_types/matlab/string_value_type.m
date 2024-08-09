% string_value_type.m - Example of using string value types

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Strings
% property string f_string
f_string_read = c.f_string;
disp(f_string_read);
% assert is used for the rest of the example property read operations
% to demonstrate the expected values.
assert (strcmp(c.f_string, 'An example string read from the service'));
c.f_string = 'An example string written to the service';

RobotRaconteur.DisconnectService(c);

% varvalue_value_types.m - Example of using varvalue wildcard type

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Any valid Robot Raconteur data type can be stored in a varvalue
% In Matlab, the RobotRaconteurVarValue() class is used to store
% the value and type of the data. The type is stored as a string.
% This is necessary because Matlab does not carry enough type
% information

% property varvalue p_varvalue_double_array
% Example of storing a double array in a varvalue
a = [1.1, 2.2, 3.3]';
c.p_varvalue_double_array = RobotRaconteurVarValue(a, "double[]");
b = c.p_varvalue_double_array;
assert(isequal(b.data, [1.4, 2.5, 3.6]'));
assert(strcmp(b.datatype, "double[]"));

% property varvalue q_varvalue_string
% Example of storing a string in a varvalue
c.q_varvalue_string = RobotRaconteurVarValue('varvalue string from client', 'string');
d = c.q_varvalue_string;
assert(strcmp(d.data, 'varvalue string from service'));
assert(strcmp(d.datatype, 'string'));

% property varvalue r_varvalue_struct
% Example of storing a structure in a varvalue
s = RobotRaconteur.CreateStructure(c, 'experimental.value_types.MyStructure');
s.a = 5;
s.b = uint32([10, 20]');
s.c = 'String from structure client';
s.d = {'string a', 'string b'}';

c.r_varvalue_struct = RobotRaconteurVarValue(s, "experimental.value_types.MyStructure");

t = c.r_varvalue_struct;
t_data = t.data;

assert(t_data.a == 52);
assert(isequal(t_data.b, [110, 120]'));
assert(strcmp(t_data.c, 'String from structure service'));
assert(length(t_data.d) == 3);
assert(strcmp(t_data.d{1}, "string c"));
assert(strcmp(t_data.d{2}, "string d"));
assert(strcmp(t_data.d{3}, 'string e'));

% property varvalue{string} s_varvalue_map2
% Example of storing a map of strings in a varvalue
% varvalue{string} is used extensively in structures
% to allow for additional fields to be added to the
% structure without changing the structure definition
c.s_varvalue_map2 = containers.Map(                       ...
    {'key1', 'key2'},                                     ...
    { RobotRaconteurVarValue(int32([2, 3]'), 'int32[]'), ...
      RobotRaconteurVarValue('string 2', 'string')}       ...
);

u = c.s_varvalue_map2;
assert(length(u) == 2)
assert(isequal(u('key3').data, [4, 5]'));
assert(strcmp(u('key3').datatype, 'int32[]'));
assert(strcmp(u('key4').data, 'string 4'));
assert(strcmp(u('key4').datatype, 'string'));

RobotRaconteur.DisconnectService(c);

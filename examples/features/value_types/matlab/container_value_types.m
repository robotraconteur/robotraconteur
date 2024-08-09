% container_value_types.m - Example of using container value types (maps and lists)

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% property double{int32} l_double_map
% int32 keyed map of scalar doubles
% Robot Raconteur Matlab uses containers.Map to store maps
c.l_double_map = containers.Map(   ...
    {int32(1), int32(2),           ...
    int32(5)},{1.1, 2.2, 3.3}      ...
);

b = c.l_double_map;
assert (length(b) == 2)
assert (b(1) == 5.1)
assert (b(5) == 6.2)

% property double[]{string} l_double_array_map
% string keyed map of double arrays
c.l_double_array_map = containers.Map(     ...
    {'key1', 'key2'},                      ...
    {[1.1, 1.2, 1.3]', [2.1, 2.2, 2.3]'}      ...
);

d = c.l_double_array_map;
assert (length(d) == 2)
assert(isequal(d('key3'), [5.1, 5.2, 5.3]'));
assert(isequal(d('key4'), [6.1, 6.2, 6.3]'));

% property string{list} m_string_list
% list of strings
% Robot Raconteur Matlab uses column cell arrays to store lists
c.m_string_list = {'string 1', 'string 2', 'string 3'}';

a = c.m_string_list;
assert (length(a) == 2);
assert (strcmp(a{1}, 'string 4'));
assert (strcmp(a{2}, 'string 5'));


% property string{int32} m_string_map_int32
% int32 keyed map of strings
c.m_string_map_int32 = containers.Map(    ...
    {int32(12), int32(100), int32(17)},   ...
    {'string 1', 'string 2', 'string 3'}  ...
);

e = c.m_string_map_int32;
assert (length(e) == 2)
assert (strcmp(e(87), 'string 4'));
assert (strcmp(e(1), 'string 5'));

% property string{string} m_string_map_string
% string keyed map of strings
c.m_string_map_string = containers.Map(   ...
    {'key1', 'key2'},                     ...
    {'string 1', 'string 2'}              ...
);

f = c.m_string_map_string;
assert (length(f) == 3)
assert (strcmp(f('key3'), 'string 3'));
assert (strcmp(f('key4'), 'string 4'));
assert (strcmp(f('key5'), 'string 5'));

% property MyVector3{int32} n_vector_map
% int32 keyed map of MyVector3
e = [1.0, 2.0, 3.0]';
c.n_vector_map = containers.Map({int32(1)}, {e});

g = c.n_vector_map;
assert (length(g) == 1);
assert(isequal(g(1), [4.0, 5.0, 6.0]'));

% property MyStructure{list} o_struct_list
% list of MyStructure
h = RobotRaconteur.CreateStructure(c, "experimental.value_types.MyStructure");
h.a = 5;
h.b = uint32([10, 20]');
h.c = 'String from structure client';
h.d = {'string a', 'string b'}';
c.o_struct_list = {h};

i = c.o_struct_list;
assert (length(i) == 1)
i0 = i{1};
assert(i0.a == 52)
assert(isequal(i0.b, [110, 120]'))
assert(strcmp(i0.c, 'String from structure service'));
assert(length(i0.d) == 3);
assert(strcmp(i0.d{1}, 'string c'));
assert(strcmp(i0.d{2}, 'string d'));
assert(strcmp(i0.d{3}, 'string e'));

RobotRaconteur.DisconnectService(c);

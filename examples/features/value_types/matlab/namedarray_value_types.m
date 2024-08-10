% namedarray_value_types.m - Example of using namedarray value types

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Robot Raconteur uses the array format for namedarray in Matlab

% property MyVector3 g_vector

c.g_vector = [1.0, 2.0, 3.0]';
b=c.g_vector;
assert (isequal(b,[4.0, 5.0, 6.0]'));

% Now an array of NamedArrays
% property MyVector3[] g_vector_array
f = [1.0,2.0,3.0;4.0,5.0,6.0]';
c.g_vector_array = f;

g = c.g_vector_array;
assert(isequal(g,[7.0,8.0,9.0;10.0,11.0,12.0]'));

% property MyQuaternion h_quaternion
j = [1.0, 0.0, 0.0, 0.0]';
c.h_quaternion = j;

k = c.h_quaternion;
assert(isequal(k, [0.707, 0.0, 0.707, 0.0]'));

% MyPose is a composite NamedArray with fields of other NamedArrays
% property MyPose l_pose
l = [1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0]';
c.i_pose = l;

m = c.i_pose;
assert(isequal(m, [0.707, 0.0, 0.707, 0.0, 4.0, 5.0, 6.0]'));

RobotRaconteur.DisconnectService(c);

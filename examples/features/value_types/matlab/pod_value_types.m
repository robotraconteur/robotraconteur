% pod_value_types.m - Example of using pod value types

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Example using Pods. Pods are represented as Matlab struct

% property MyPod j_pod
a = struct;
a.a = 0.928;
a.b = int32(8374);
a.c = uint8([8, 9, 10, 11]');
a.d = uint8(17);
a.e = uint16([1, 2, 3; 4, 5, 6]);
a.f = [10.1, 10.2, 10.3]';
c.j_pod = a;

b = c.j_pod;
assert (b.a == 0.791)
assert (b.b == 1077)
assert(isequal(b.c, [61, 52, 33, 24]'));
assert(isequal(b.d, [1, 2]'));
assert(isequal(b.e, [7, 8, 9; 10, 11, 12]));
assert (isequal(b.f, [20.1, 20.2, 20.3]'));

% Now an array of Pods
% property MyPod[] j_pod_array
d = struct;
d.a = 0.928;
d.b = int32(8374);
d.c = uint8([8, 9, 10, 11]');
d.d = uint8(17);
d.e = uint16([1, 2, 3; 4, 5, 6]);
d.f = [10.1, 10.2, 10.3]';
d2 = struct;
d2.a = 0.67;
d2.b = int32(123);
d2.c = uint8([1, 2, 3, 4]');
d2.d = uint8([5, 6]');
d2.e = uint16([13, 14, 15; 16, 17, 18]);
d2.f = [30.1, 30.2, 30.3]';
d(2,1) = d2;
c.j_pod_array = d;

e = c.j_pod_array;
assert(e(1).a == 0.791);
assert(e(1).b == 1077);
assert(isequal(e(1).c, [61, 52, 33, 24]'));
assert(isequal(e(1).d, [1, 2]'));
assert(isequal(e(1).e, [7, 8, 9; 10, 11, 12]));
assert(isequal(e(1).f, [20.1, 20.2, 20.3]'));
assert(e(2).a == 0.03);
assert(e(2).b == 693);
assert(isequal(e(2).c, [5, 6, 7, 8]'));
assert(isequal(e(2).d, 3));
assert(isequal(e(2).e, [19, 20, 21; 22, 23, 24]));
assert(isequal(e(2).f, [40.1, 40.2, 40.3]'));

RobotRaconteur.DisconnectService(c);

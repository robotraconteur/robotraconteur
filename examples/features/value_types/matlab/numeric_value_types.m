% numeric_value_types.m - Example of using numeric value types

c = RobotRaconteur.ConnectService('rr+tcp://localhost:53223?service=values_example');

% Scalar numbers
% property double a_double
a_double_read = c.a_double;
disp(a_double_read);
% assert is used for the rest of the example property read operations
% to demonstrate the expected values.
assert(c.a_double == 5.78);
c.a_double = 49.3;
% property int32 b_int
assert (c.b_int == 4557)
c.b_int = int32(359);
% property uint8 c_byte
assert (c.c_byte == 0x1A)
c.c_byte = uint8(31);
% property cdouble d_cdouble
assert (c.d_cdouble == 23.7 + 5.3i)
c.d_cdouble = 1.2 + 3.4i;
% property bool e_bool
assert (c.e_bool == true)
c.e_bool = false;
% property int32 meaning_of_life [readonly]
assert (c.meaning_of_life == 42)

% Numeric Arrays
% property double[] a_double_array
assert(isequal(c.a_double_array, [0.016, 0.226]'));
c.a_double_array = [0.582, 0.288, 0.09, 0.213, 0.98]';
% property double[3] a_double_array_fixed
assert(isequal(c.a_double_array_fixed, [0.13, 0.27, 0.15]'));
c.a_double_array_fixed = [0.21,0.12, 0.39]';
% property double[6-] a_double_array_maxlen
assert(isequal(c.a_double_array_maxlen, [0.7, 0.16, 0.16, 0.05, 0.61, 0.9]'));
c.a_double_array_maxlen = [0.035, 0.4]';
% property double[3,2] a_double_marray_fixed
assert(isequal(c.a_double_marray_fixed, [0.29, 0.66; 0.41, 0.6; 0.4, 0.2]));
c.a_double_marray_fixed = [0.3, 0.6; 0.4, 0.6; 0.5, 0.2];
% property double[*] a_double_marray
assert(isequal(c.a_double_marray, [0.72, 0.4; 0.05, 0.07]));
c.a_double_marray = [0.3; 0.01];
% property uint8[] c_byte_array
assert(isequal(c.c_byte_array, [0x1A, 0x2B]'));
c.c_byte_array = uint8([0x3C, 0x4D, 0x5E, 0x6F, 0x70]');

RobotRaconteur.DisconnectService(c);

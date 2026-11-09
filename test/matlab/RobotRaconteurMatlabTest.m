function RobotRaconteurMatlabTest(url)


c=RobotRaconteur.Connect(url);

% cSpell: ignore lstruct

c.d1 = 3.456;
assert (c.d1 == 12.345)

c.d2 = test_vectors_get('ts1.d2.to_service', 'double')';
ca(c.d2, test_vectors_get('ts1.d2.to_client', 'double')')
c.d3 = [9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01, 1.969554e+12, 9.451979e-05, 5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13, 2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14]';
ca(c.d3, [2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11, 9.818531e-21, 6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00, 3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21]')
c.d4 = [-4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11, 3.511191e-08, 4.759250e-07, 2.455883e+09, 4.182578e+11, 4.732337e-14, -2.967313e+02, -4.139188e+14, 6.287269e+03]';
ca(c.d4, [2.864760e-08, 3.900663e+13, 9.105789e+11, 2.943743e-15, -2.823159e-16, -3.481261e+19]')

c.d5=reshape(test_vectors_get('ts1.d5.to_service', 'double'),[8, 6, 2]);
ca(c.d5,reshape(test_vectors_get('ts1.d5.to_client', 'double'),[5, 6, 5]))
c.s1 = single(3847.9283);

assert (c.s1 == single(7.8573))

c.s2 = test_vectors_get('ts1.s2.to_service', 'single')';
ca(c.s2, test_vectors_get('ts1.s2.to_client', 'single')')
c.i8_1 = int8(45);
assert (c.i8_1 == -66)

c.i8_2 = test_vectors_get('ts1.i8_2.to_service', 'int8')';
ca(c.i8_2, test_vectors_get('ts1.i8_2.to_client', 'int8')')
c.u8_1 = uint8(232);
assert (c.u8_1 == 222)

c.u8_2 = test_vectors_get('ts1.u8_2.to_service', 'uint8')';
ca(c.u8_2, test_vectors_get('ts1.u8_2.to_client', 'uint8')')
c.u8_3=reshape(test_vectors_get('ts1.u8_3.to_service', 'uint8'),[30, 10]);
ca(c.u8_3,reshape(test_vectors_get('ts1.u8_3.to_client', 'uint8'),[10,20]))

c.i16_1 = int16(2387);
assert (c.i16_1 == -13428)

c.i16_2 = test_vectors_get('ts1.i16_2.to_service', 'int16')';
ca(c.i16_2, test_vectors_get('ts1.i16_2.to_client', 'int16')')
c.u16_1 = uint16(54732);
assert (c.u16_1 == 60981)

c.u16_2 = test_vectors_get('ts1.u16_2.to_service', 'uint16')';
ca(c.u16_2, test_vectors_get('ts1.u16_2.to_client', 'uint16')')

c.i32_1 = int32(-9837284);
assert (c.i32_1 == 898734)

c.i32_2 = test_vectors_get('ts1.i32_2.to_service', 'int32')';
ca(c.i32_2, test_vectors_get('ts1.i32_2.to_client', 'int32')')
c.u32_1 = uint32(1550099195);
assert (c.u32_1 == 547919123)

c.u32_2 = test_vectors_get('ts1.u32_2.to_service', 'uint32')';
ca(c.u32_2, test_vectors_get('ts1.u32_2.to_client', 'uint32')')

c.i64_1 = int64(8621740821050813024);
assert (c.i64_1 == int64(-1357833931563696072))

%c.i64_2 = test_vectors_get('ts1.i64_2.to_service', 'int64')';
%ca(c.i64_2, test_vectors_get('ts1.i64_2.to_client', 'int64')')
%c.u64_1 = uint64(1465640522145789825);
assert (c.u64_1 == uint64(13389861970863644378))

%c.u64_2 = test_vectors_get('ts1.u64_2.to_service', 'uint64')';
%ca(c.u64_2, test_vectors_get('ts1.u64_2.to_client', 'uint64')')

c.str1='Hello Server!';
ca(c.str1,'Hello Client!')

%test the big structure
s1=c.struct1;
ca(s1.dat1,test_vectors_get('ts1.struct1.dat1.to_client', 'double')')
ca(s1.str2,'Hello world!')
assert (size(s1.vec3,1)==3)

ca(s1.vec3(1),'Hello Client!')
ca(s1.vec3(2),'Hello Client, again')
ca(s1.vec3(4372),'This is yet another test string')
ca(s1.dict4('teststring1'),'Hello Client!')
ca(s1.dict4('teststring2'),'Hello Client, again')
ca(s1.dict4('anotherstr'),'This is yet another test string')
s1_s2=s1.struct1;
ca(s1_s2.mydat,test_vectors_get('ts1.struct1.struct1_mydat.to_client', 'double')')

s1_1=s1.dstruct2('test1');
s1_2=s1.dstruct2( 'anothertest');
ca(s1_1.mydat, test_vectors_get('ts1.struct1.dstruct2_1.to_client', 'double')')
ca(s1_2.mydat, [4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08, -4.969544e-21, 2.530419e-01, 4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17, 1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02]')

ca(s1.multidimarray,reshape(test_vectors_get('ts1.struct1.multidimarray.to_client', 'double'),[10,10]))
ca(s1.var3.data,'This is a vartype string')

s2=struct;
s2.dat1 = test_vectors_get('ts1.struct1.dat1.to_service', 'double')';
s2.str2 = 'Hello world 2!';
s2.vec3=containers.Map({int32(10),int32(11),int32(46372),int32(46373)},{'Hello Server!','Hello Server, again','Test string!','Test string again'});
s2.dict4=containers.Map({'cteststring1','cteststring2'},{'Hello Server!','Hello Server, again'});
s2.list5={'Hello Server!';'Hello Server, again'};
s2_s2=struct;
s2_s2.mydat=test_vectors_get('ts1.struct1.struct1_mydat.to_service', 'double')';
s2.struct1=s2_s2;

s2_ds2_1=struct;
s2_ds2_1.mydat=test_vectors_get('ts1.struct1.dstruct2_1.to_service', 'double')';
s2_ds2_2=struct;
s2_ds2_2.mydat=test_vectors_get('ts1.struct1.dstruct2_2.to_service', 'double')';
s2.dstruct2=containers.Map({'ctest1','anothertest'},{s2_ds2_1,s2_ds2_2});
s2_ls3_1=struct;
s2_ls3_1.mydat=test_vectors_get('ts1.struct1.dstruct2_1.to_service', 'double')';
s2_ls3_2=struct;
s2_ls3_2.mydat=test_vectors_get('ts1.struct1.dstruct2_2.to_service', 'double')';
s2.lstruct3={s2_ls3_1;s2_ls3_2};
s2.multidimarray=reshape(test_vectors_get('ts1.struct1.multidimarray.to_service', 'double'),[10,10]);
s2.var3=RobotRaconteurVarValue([6.404176e-12, 9.258110e-03, 8.657620e-03, -2.064381e+00, 5.182360e-16, 4.167658e-16, -4.533051e-19, 5.357520e+18, -4.990383e-13, 2.286982e+08, -4.727256e-18, 1.465299e-17, 3.000340e-10, -2.304453e-04]','double[]');

c.struct1=s2;

s1_s3=c.struct3;
ca(s1_s3.a1,[-8.483090e-19, -4.401548e-08, 3.908118e+00, 2.063513e-18, 4.237047e+18, -1.124681e-16, 3.924541e-01, -2.184335e-10, -1.978950e+11, 1.586365e+18, 1.712393e+00, -6.314723e+00, 1.196777e-16, -2.748704e-08, -1.289967e+02, -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06, 1.315398e+17]')
s1_s3_2=struct;
s1_s3_2.a1=[-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03, 8.256717e-12, -4.772119e-11, -1.061407e-13, 2.759750e+02, -1.212549e+10, 7.012690e+15, 3.953354e+04, -2.617985e-07, 1.104408e-21, -3.889366e+00, 4.549493e+16, -1.376791e+15, -3.445205e-21, 2.137830e-14, 4.620179e+18]';
c.struct3=s1_s3_2;

var_num_1i = c.var_num;
ca(var_num_1i.datatype,'int32[]')
ca(var_num_1i.data,[-1680284833, -54562307, 732107275, 1470526962, -1389452949, 256801409, 261288152, 1728150828, 1322531658, -1640628174, 1036878614, 511108054, 2057847386, 288780916, 996595759]')
c.var_num=RobotRaconteurVarValue(int32([-1046369769, 1950632347, 1140727074, -1277424443, 163999900, 970815027, 545593183, 514305170, 1896372264, 1385916382]'),'int32[]');
var_str_1i = c.var_str;
ca(var_str_1i.datatype,'string')
ca(var_str_1i.data,'Hello Client!')
c.var_str=RobotRaconteurVarValue('Hello Server!','string');
var_struct_1i=c.var_struct;
ca(var_struct_1i.datatype,'com.robotraconteur.testing.TestService1.teststruct2')
ca(var_struct_1i.data.mydat,[-9.052731e+13, 4.151705e-17, -4.004463e+19, -2.838274e+03, 9.983314e+12, 2.764122e+10, -1.131486e+03, 2.418899e+12, 1.323675e-05, -4.602174e+13, 2.717530e+01, 1.193887e-10, -4.137578e+16, -1.246990e-19, 4.244315e-18, -2.833005e-08, 1.956266e-04, 4.130129e-21, 1.641708e-11, -4.488158e-19]')
var_struct_1=struct;
var_struct_1.mydat=[-4.945426e-20, 1.763386e+13, 3.431578e-04, 4.411409e+17, -2.690201e+03, 3.025939e-10, -3.659846e+11, -4.780435e-10, -3.246816e+14, -1.815578e+04, 2.236455e+10, -4.639041e+14, 1.767930e+10, -1.636094e+05, -4.392462e-01, 2.225260e+04, -5.250245e+18, 8.755282e-12, 2.005819e-10, 2.702210e+04]';
c.var_struct=RobotRaconteurVarValue(var_struct_1,'com.robotraconteur.testing.TestService1.teststruct2');

c.nulltest=missing;
assert(ismissing(c.nulltest));

c.func1();
c.func2(1,2);
assert (c.func3(3,4)==7)


RobotRaconteur.Disconnect(c)
disp('Done!')


function ca(d1,d2)
    assert ((all(d1==d2,'all')))
end


end

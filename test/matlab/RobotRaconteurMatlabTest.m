function RobotRaconteurMatlabTest(url)


c=RobotRaconteur.Connect(url);

% cSpell: ignore lstruct

c.d1 = 3.456;
assert (c.d1 == 12.345)

c.d2 = test_vectors_get('ts1.d2.to_service', 'double')';
ca(c.d2, test_vectors_get('ts1.d2.to_client', 'double')')
c.d3 = test_vectors_get('ts1.d3.to_service', 'double')';
ca(c.d3, test_vectors_get('ts1.d3.to_client', 'double')')
c.d4 = test_vectors_get('ts1.d4.to_service', 'double')';
ca(c.d4, test_vectors_get('ts1.d4.to_client', 'double')')

c.d5=reshape(test_vectors_get('ts1.d5.to_service', 'double'),double(test_vectors_get('ts1.d5.to_service.dims', 'uint32')));
ca(c.d5,reshape(test_vectors_get('ts1.d5.to_client', 'double'),double(test_vectors_get('ts1.d5.to_client.dims', 'uint32'))))
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
c.u8_3=reshape(test_vectors_get('ts1.u8_3.to_service', 'uint8'),double(test_vectors_get('ts1.u8_3.to_service.dims', 'uint32')));
ca(c.u8_3,reshape(test_vectors_get('ts1.u8_3.to_client', 'uint8'),double(test_vectors_get('ts1.u8_3.to_client.dims', 'uint32'))))

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
ca(s1_2.mydat, test_vectors_get('ts1.struct1.dstruct2_3.to_client', 'double')')

ca(s1.multidimarray,reshape(test_vectors_get('ts1.struct1.multidimarray.to_client', 'double'),double(test_vectors_get('ts1.struct1.multidimarray.to_client.dims', 'uint32'))))
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
s2.multidimarray=reshape(test_vectors_get('ts1.struct1.multidimarray.to_service', 'double'),double(test_vectors_get('ts1.struct1.multidimarray.to_service.dims', 'uint32')));
s2.var3=RobotRaconteurVarValue(test_vectors_get('ts1.struct1.var3.to_service', 'double')','double[]');

c.struct1=s2;

s1_s3=c.struct3;
ca(s1_s3.a1,test_vectors_get('ts1.struct3.to_client', 'double')')
s1_s3_2=struct;
s1_s3_2.a1=test_vectors_get('ts1.struct3.to_service', 'double')';
c.struct3=s1_s3_2;

var_num_1i = c.var_num;
ca(var_num_1i.datatype,'int32[]')
ca(var_num_1i.data,test_vectors_get('ts1.var_num.to_client', 'int32')')
c.var_num=RobotRaconteurVarValue(test_vectors_get('ts1.var_num.to_service', 'int32')','int32[]');
var_str_1i = c.var_str;
ca(var_str_1i.datatype,'string')
ca(var_str_1i.data,'Hello Client!')
c.var_str=RobotRaconteurVarValue('Hello Server!','string');
var_struct_1i=c.var_struct;
ca(var_struct_1i.datatype,'com.robotraconteur.testing.TestService1.teststruct2')
ca(var_struct_1i.data.mydat,test_vectors_get('ts1.var_struct.to_client', 'double')')
var_struct_1=struct;
var_struct_1.mydat=test_vectors_get('ts1.var_struct.to_service', 'double')';
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

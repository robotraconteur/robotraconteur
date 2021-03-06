//This file is automatically generated. DO NOT EDIT!
package com.robotraconteur.testing.TestService1;
import java.util.*;
import com.robotraconteur.*;
@RobotRaconteurServiceObjectInterface
public interface testroot extends com.robotraconteur.testing.TestService2.baseobj
{
    double get_d1();
    void set_d1(double value);
    double[] get_d2();
    void set_d2(double[] value);
    double[] get_d3();
    void set_d3(double[] value);
    double[] get_d4();
    void set_d4(double[] value);
    MultiDimArray get_d5();
    void set_d5(MultiDimArray value);
    MultiDimArray get_d6();
    void set_d6(MultiDimArray value);
    float get_s1();
    void set_s1(float value);
    float[] get_s2();
    void set_s2(float[] value);
    byte get_i8_1();
    void set_i8_1(byte value);
    byte[] get_i8_2();
    void set_i8_2(byte[] value);
    UnsignedByte get_u8_1();
    void set_u8_1(UnsignedByte value);
    UnsignedBytes get_u8_2();
    void set_u8_2(UnsignedBytes value);
    MultiDimArray get_u8_3();
    void set_u8_3(MultiDimArray value);
    short get_i16_1();
    void set_i16_1(short value);
    short[] get_i16_2();
    void set_i16_2(short[] value);
    UnsignedShort get_u16_1();
    void set_u16_1(UnsignedShort value);
    UnsignedShorts get_u16_2();
    void set_u16_2(UnsignedShorts value);
    int get_i32_1();
    void set_i32_1(int value);
    int[] get_i32_2();
    void set_i32_2(int[] value);
    int[] get_i32_huge();
    void set_i32_huge(int[] value);
    UnsignedInt get_u32_1();
    void set_u32_1(UnsignedInt value);
    UnsignedInts get_u32_2();
    void set_u32_2(UnsignedInts value);
    long get_i64_1();
    void set_i64_1(long value);
    long[] get_i64_2();
    void set_i64_2(long[] value);
    UnsignedLong get_u64_1();
    void set_u64_1(UnsignedLong value);
    UnsignedLongs get_u64_2();
    void set_u64_2(UnsignedLongs value);
    String get_str1();
    void set_str1(String value);
    teststruct1 get_struct1();
    void set_struct1(teststruct1 value);
    teststruct2 get_struct2();
    void set_struct2(teststruct2 value);
    Map<Integer,double[]> get_is_d1();
    void set_is_d1(Map<Integer,double[]> value);
    Map<String,double[]> get_is_d2();
    void set_is_d2(Map<String,double[]> value);
    Map<Integer,double[]> get_is_d3();
    void set_is_d3(Map<Integer,double[]> value);
    Map<String,double[]> get_is_d4();
    void set_is_d4(Map<String,double[]> value);
    Map<Integer,MultiDimArray> get_is_d5();
    void set_is_d5(Map<Integer,MultiDimArray> value);
    Map<String,MultiDimArray> get_is_d6();
    void set_is_d6(Map<String,MultiDimArray> value);
    Map<Integer,String> get_is_str1();
    void set_is_str1(Map<Integer,String> value);
    Map<String,String> get_is_str2();
    void set_is_str2(Map<String,String> value);
    Map<Integer,teststruct2> get_is_struct1();
    void set_is_struct1(Map<Integer,teststruct2> value);
    Map<String,teststruct2> get_is_struct2();
    void set_is_struct2(Map<String,teststruct2> value);
    com.robotraconteur.testing.TestService2.ostruct2 get_struct3();
    void set_struct3(com.robotraconteur.testing.TestService2.ostruct2 value);
    List<double[]> get_list_d1();
    void set_list_d1(List<double[]> value);
    List<double[]> get_list_d3();
    void set_list_d3(List<double[]> value);
    List<MultiDimArray> get_list_d5();
    void set_list_d5(List<MultiDimArray> value);
    List<String> get_list_str1();
    void set_list_str1(List<String> value);
    List<teststruct2> get_list_struct1();
    void set_list_struct1(List<teststruct2> value);
    Object get_var1();
    void set_var1(Object value);
    Map<Integer,Object> get_var2();
    void set_var2(Map<Integer,Object> value);
    Object get_var_num();
    void set_var_num(Object value);
    Object get_var_str();
    void set_var_str(Object value);
    Object get_var_struct();
    void set_var_struct(Object value);
    Object get_var_vector();
    void set_var_vector(Object value);
    Object get_var_dictionary();
    void set_var_dictionary(Object value);
    Object get_var_list();
    void set_var_list(Object value);
    Object get_var_multidimarray();
    void set_var_multidimarray(Object value);
    double get_errtest();
    void set_errtest(double value);
    teststruct1 get_nulltest();
    void set_nulltest(teststruct1 value);
    void func1();
    void func2(double d1, double d2);
    double func3(double d1, double d2);
    int meaning_of_life();
    void func_errtest();
    void func_errtest1();
    void func_errtest2();
    void func_errtest3();
    void o6_op(int op);
    void pipe_check_error();
    void test_callbacks();
    void  addev1Listener(Action listener); 
    void  removeev1Listener(Action listener); 
    void  addev2Listener(Action2<Double, teststruct2> listener); 
    void  removeev2Listener(Action2<Double, teststruct2> listener); 
    sub1 get_o1();
    sub1 get_o2(int ind);
    sub1 get_o3(int ind);
    sub1 get_o4(String ind);
    com.robotraconteur.testing.TestService2.subobj get_o5();
    Object get_o6();
    Pipe<double[]> get_p1();
    void set_p1(Pipe<double[]> value);
    Pipe<teststruct2> get_p2();
    void set_p2(Pipe<teststruct2> value);
    Pipe<double[]> get_broadcastpipe();
    void set_broadcastpipe(Pipe<double[]> value);
    Callback<Action> get_cb1();
    void set_cb1(Callback<Action> value);
    Callback<Action2<Double, Double>> get_cb2();
    void set_cb2(Callback<Action2<Double, Double>> value);
    Callback<Func2<Double,Double,Double>> get_cb3();
    void set_cb3(Callback<Func2<Double,Double,Double>> value);
    Callback<Func<Integer>> get_cb_meaning_of_life();
    void set_cb_meaning_of_life(Callback<Func<Integer>> value);
    Callback<Action> get_cb_errtest();
    void set_cb_errtest(Callback<Action> value);
    Wire<double[]> get_w1();
    void set_w1(Wire<double[]> value);
    Wire<teststruct2> get_w2();
    void set_w2(Wire<teststruct2> value);
    Wire<MultiDimArray> get_w3();
    void set_w3(Wire<MultiDimArray> value);
    Wire<double[]> get_broadcastwire();
    void set_broadcastwire(Wire<double[]> value);
    ArrayMemory<double[]> get_m1();
    MultiDimArrayMemory<double[]> get_m2();
    MultiDimArrayMemory<UnsignedBytes> get_m3();
}


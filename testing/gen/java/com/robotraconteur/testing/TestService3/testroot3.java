//This file is automatically generated. DO NOT EDIT!
package com.robotraconteur.testing.TestService3;
import java.util.*;
import com.robotraconteur.*;
@RobotRaconteurServiceObjectInterface
public interface testroot3
{
    int get_readme();
    void set_writeme(int value);
    int get_unknown_modifier();
    void set_unknown_modifier(int value);
    testenum1 get_testenum1_prop();
    void set_testenum1_prop(testenum1 value);
    testpod1 get_testpod1_prop();
    void set_testpod1_prop(testpod1 value);
    teststruct3 get_teststruct3_prop();
    void set_teststruct3_prop(teststruct3 value);
    List<double[]> get_d1();
    void set_d1(List<double[]> value);
    List<double[]> get_d2();
    void set_d2(List<double[]> value);
    Map<Integer,double[]> get_d3();
    void set_d3(Map<Integer,double[]> value);
    Map<Integer,double[]> get_d4();
    void set_d4(Map<Integer,double[]> value);
    List<MultiDimArray> get_d5();
    void set_d5(List<MultiDimArray> value);
    Map<Integer,MultiDimArray> get_d6();
    void set_d6(Map<Integer,MultiDimArray> value);
    vector3 get_testnamedarray1();
    void set_testnamedarray1(vector3 value);
    transform get_testnamedarray2();
    void set_testnamedarray2(transform value);
    transform[] get_testnamedarray3();
    void set_testnamedarray3(transform[] value);
    NamedMultiDimArray get_testnamedarray4();
    void set_testnamedarray4(NamedMultiDimArray value);
    NamedMultiDimArray get_testnamedarray5();
    void set_testnamedarray5(NamedMultiDimArray value);
    CDouble get_c1();
    void set_c1(CDouble value);
    CDouble[] get_c2();
    void set_c2(CDouble[] value);
    MultiDimArray get_c3();
    void set_c3(MultiDimArray value);
    List<CDouble[]> get_c4();
    void set_c4(List<CDouble[]> value);
    List<CDouble[]> get_c5();
    void set_c5(List<CDouble[]> value);
    List<MultiDimArray> get_c6();
    void set_c6(List<MultiDimArray> value);
    CSingle get_c7();
    void set_c7(CSingle value);
    CSingle[] get_c8();
    void set_c8(CSingle[] value);
    MultiDimArray get_c9();
    void set_c9(MultiDimArray value);
    List<CSingle[]> get_c10();
    void set_c10(List<CSingle[]> value);
    List<CSingle[]> get_c11();
    void set_c11(List<CSingle[]> value);
    List<MultiDimArray> get_c12();
    void set_c12(List<MultiDimArray> value);
    boolean get_b1();
    void set_b1(boolean value);
    boolean[] get_b2();
    void set_b2(boolean[] value);
    MultiDimArray get_b3();
    void set_b3(MultiDimArray value);
    List<boolean[]> get_b4();
    void set_b4(List<boolean[]> value);
    List<boolean[]> get_b5();
    void set_b5(List<boolean[]> value);
    List<MultiDimArray> get_b6();
    void set_b6(List<MultiDimArray> value);
    void testpod1_func1(testpod1 s);
    testpod1 testpod1_func2();
    Generator2<double[]> gen_func1();
    Generator2<UnsignedBytes> gen_func2(String name);
    Generator3<UnsignedBytes> gen_func3(String name);
    Generator1<UnsignedBytes,UnsignedBytes> gen_func4();
    Generator1<com.robotraconteur.testing.TestService1.teststruct2,com.robotraconteur.testing.TestService1.teststruct2> gen_func5();
    obj4 get_o4();
    obj5 get_nolock_test();
    Pipe<int[]> get_unreliable1();
    void set_unreliable1(Pipe<int[]> value);
    Pipe<int[]> get_p1();
    void set_p1(Pipe<int[]> value);
    Pipe<int[]> get_p2();
    void set_p2(Pipe<int[]> value);
    Pipe<MultiDimArray> get_p3();
    void set_p3(Pipe<MultiDimArray> value);
    Wire<int[]> get_peekwire();
    void set_peekwire(Wire<int[]> value);
    Wire<int[]> get_pokewire();
    void set_pokewire(Wire<int[]> value);
    Wire<int[]> get_w1();
    void set_w1(Wire<int[]> value);
    Wire<int[]> get_w2();
    void set_w2(Wire<int[]> value);
    Wire<MultiDimArray> get_w3();
    void set_w3(Wire<MultiDimArray> value);
    ArrayMemory<double[]> get_readmem();
    PodArrayMemory<testpod2[]> get_pod_m1();
    PodMultiDimArrayMemory<testpod2[]> get_pod_m2();
    NamedArrayMemory<transform[]> get_namedarray_m1();
    NamedMultiDimArrayMemory<transform[]> get_namedarray_m2();
    ArrayMemory<CDouble[]> get_c_m1();
    MultiDimArrayMemory<CDouble[]> get_c_m2();
    ArrayMemory<CDouble[]> get_c_m3();
    MultiDimArrayMemory<CDouble[]> get_c_m4();
    ArrayMemory<boolean[]> get_c_m5();
    MultiDimArrayMemory<boolean[]> get_c_m6();
}


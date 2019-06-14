//This file is automatically generated. DO NOT EDIT!
package com.robotraconteur.testing.TestService3;
import java.util.*;
import com.robotraconteur.*;
public class testroot3_skel extends ServiceSkel {
    protected testroot3 obj;
    public testroot3_skel(Object o) { 
    super(o);
    obj=(testroot3)o;
    }
    public void releaseCastObject() { 
    obj=null;
    super.releaseCastObject();
    }
    public MessageElement callGetProperty(String membername) {
    if(membername.equals( "readme"))
    {
    int ret=obj.get_readme();
    return MessageElementUtil.<int[]>packArray("return",new int[] {ret});
    }
    if(membername.equals( "unknown_modifier"))
    {
    int ret=obj.get_unknown_modifier();
    return MessageElementUtil.<int[]>packArray("return",new int[] {ret});
    }
    if(membername.equals( "testenum1_prop"))
    {
    testenum1 ret=obj.get_testenum1_prop();
    return MessageElementUtil.<int[]>packArray("return", new int[] {((int)ret.getValue())});
    }
    if(membername.equals( "testpod1_prop"))
    {
    testpod1 ret=obj.get_testpod1_prop();
    return MessageElementUtil.<testpod1>packPodToArray("return",ret);
    }
    if(membername.equals( "teststruct3_prop"))
    {
    teststruct3 ret=obj.get_teststruct3_prop();
    return MessageElementUtil.packStructure("return",ret);
    }
    if(membername.equals( "d1"))
    {
    List<double[]> ret=obj.get_d1();
    return MessageElementUtil.<double[]>packListType("return",DataTypeUtil.verifyArrayLength1(ret, 6, false),double[].class);
    }
    if(membername.equals( "d2"))
    {
    List<double[]> ret=obj.get_d2();
    return MessageElementUtil.<double[]>packListType("return",DataTypeUtil.verifyArrayLength1(ret, 6, true),double[].class);
    }
    if(membername.equals( "d3"))
    {
    Map<Integer,double[]> ret=obj.get_d3();
    return MessageElementUtil.<Integer,double[]>packMapType("return",DataTypeUtil.verifyArrayLength1(ret, 6, false),Integer.class,double[].class);
    }
    if(membername.equals( "d4"))
    {
    Map<Integer,double[]> ret=obj.get_d4();
    return MessageElementUtil.<Integer,double[]>packMapType("return",DataTypeUtil.verifyArrayLength1(ret, 6, true),Integer.class,double[].class);
    }
    if(membername.equals( "d5"))
    {
    List<MultiDimArray> ret=obj.get_d5();
    return MessageElementUtil.<MultiDimArray>packListType("return",DataTypeUtil.verifyArrayLength2(ret,9,new int[] {3,3}),MultiDimArray.class);
    }
    if(membername.equals( "d6"))
    {
    Map<Integer,MultiDimArray> ret=obj.get_d6();
    return MessageElementUtil.<Integer,MultiDimArray>packMapType("return",DataTypeUtil.verifyArrayLength2(ret,9,new int[] {3,3}),Integer.class,MultiDimArray.class);
    }
    if(membername.equals( "testnamedarray1"))
    {
    vector3 ret=obj.get_testnamedarray1();
    return MessageElementUtil.<vector3>packNamedArrayToArray("return",ret);
    }
    if(membername.equals( "testnamedarray2"))
    {
    transform ret=obj.get_testnamedarray2();
    return MessageElementUtil.<transform>packNamedArrayToArray("return",ret);
    }
    if(membername.equals( "testnamedarray3"))
    {
    transform[] ret=obj.get_testnamedarray3();
    return MessageElementUtil.<transform>packNamedArray("return",DataTypeUtil.verifyArrayLength(ret, 10, true));
    }
    if(membername.equals( "testnamedarray4"))
    {
    NamedMultiDimArray ret=obj.get_testnamedarray4();
    return MessageElementUtil.<NamedMultiDimArray>packNamedMultiDimArray("return",ret);
    }
    if(membername.equals( "testnamedarray5"))
    {
    NamedMultiDimArray ret=obj.get_testnamedarray5();
    return MessageElementUtil.<NamedMultiDimArray>packNamedMultiDimArray("return",DataTypeUtil.verifyArrayLength(ret,6,new int[] {3,2}));
    }
    if(membername.equals( "c1"))
    {
    CDouble ret=obj.get_c1();
    return MessageElementUtil.<CDouble[]>packArray("return",new CDouble[] {ret});
    }
    if(membername.equals( "c2"))
    {
    CDouble[] ret=obj.get_c2();
    return MessageElementUtil.<CDouble[]>packArray("return",ret);
    }
    if(membername.equals( "c3"))
    {
    MultiDimArray ret=obj.get_c3();
    return MessageElementUtil.packMultiDimArray("return",(MultiDimArray)ret);
    }
    if(membername.equals( "c4"))
    {
    List<CDouble[]> ret=obj.get_c4();
    return MessageElementUtil.<CDouble[]>packListType("return",ret,CDouble[].class);
    }
    if(membername.equals( "c5"))
    {
    List<CDouble[]> ret=obj.get_c5();
    return MessageElementUtil.<CDouble[]>packListType("return",ret,CDouble[].class);
    }
    if(membername.equals( "c6"))
    {
    List<MultiDimArray> ret=obj.get_c6();
    return MessageElementUtil.<MultiDimArray>packListType("return",ret,MultiDimArray.class);
    }
    if(membername.equals( "c7"))
    {
    CSingle ret=obj.get_c7();
    return MessageElementUtil.<CSingle[]>packArray("return",new CSingle[] {ret});
    }
    if(membername.equals( "c8"))
    {
    CSingle[] ret=obj.get_c8();
    return MessageElementUtil.<CSingle[]>packArray("return",ret);
    }
    if(membername.equals( "c9"))
    {
    MultiDimArray ret=obj.get_c9();
    return MessageElementUtil.packMultiDimArray("return",(MultiDimArray)ret);
    }
    if(membername.equals( "c10"))
    {
    List<CSingle[]> ret=obj.get_c10();
    return MessageElementUtil.<CSingle[]>packListType("return",ret,CSingle[].class);
    }
    if(membername.equals( "c11"))
    {
    List<CSingle[]> ret=obj.get_c11();
    return MessageElementUtil.<CSingle[]>packListType("return",ret,CSingle[].class);
    }
    if(membername.equals( "c12"))
    {
    List<MultiDimArray> ret=obj.get_c12();
    return MessageElementUtil.<MultiDimArray>packListType("return",ret,MultiDimArray.class);
    }
    if(membername.equals( "b1"))
    {
    boolean ret=obj.get_b1();
    return MessageElementUtil.<boolean[]>packArray("return",new boolean[] {ret});
    }
    if(membername.equals( "b2"))
    {
    boolean[] ret=obj.get_b2();
    return MessageElementUtil.<boolean[]>packArray("return",ret);
    }
    if(membername.equals( "b3"))
    {
    MultiDimArray ret=obj.get_b3();
    return MessageElementUtil.packMultiDimArray("return",(MultiDimArray)ret);
    }
    if(membername.equals( "b4"))
    {
    List<boolean[]> ret=obj.get_b4();
    return MessageElementUtil.<boolean[]>packListType("return",ret,boolean[].class);
    }
    if(membername.equals( "b5"))
    {
    List<boolean[]> ret=obj.get_b5();
    return MessageElementUtil.<boolean[]>packListType("return",ret,boolean[].class);
    }
    if(membername.equals( "b6"))
    {
    List<MultiDimArray> ret=obj.get_b6();
    return MessageElementUtil.<MultiDimArray>packListType("return",ret,MultiDimArray.class);
    }
    throw new MemberNotFoundException("Member not found");
    }
    public void callSetProperty(String membername, MessageElement m) {
    if(membername.equals( "writeme"))
    {
    obj.set_writeme((MessageElementUtil.<int[]>castDataAndDispose(m))[0]);
    return;
    }
    if(membername.equals( "unknown_modifier"))
    {
    obj.set_unknown_modifier((MessageElementUtil.<int[]>castDataAndDispose(m))[0]);
    return;
    }
    if(membername.equals( "testenum1_prop"))
    {
    obj.set_testenum1_prop(testenum1.intToEnum((MessageElementUtil.<int[]>castDataAndDispose(m)[0])));
    return;
    }
    if(membername.equals( "testpod1_prop"))
    {
    obj.set_testpod1_prop(MessageElementUtil.<testpod1>unpackPodFromArray(m));
    return;
    }
    if(membername.equals( "teststruct3_prop"))
    {
    obj.set_teststruct3_prop(MessageElementUtil.<teststruct3>unpackStructure(m));
    return;
    }
    if(membername.equals( "d1"))
    {
    obj.set_d1(DataTypeUtil.verifyArrayLength1(MessageElementUtil.<double[]>unpackListType(m), 6, false));
    return;
    }
    if(membername.equals( "d2"))
    {
    obj.set_d2(DataTypeUtil.verifyArrayLength1(MessageElementUtil.<double[]>unpackListType(m), 6, true));
    return;
    }
    if(membername.equals( "d3"))
    {
    obj.set_d3(DataTypeUtil.verifyArrayLength1(MessageElementUtil.<Integer,double[]>unpackMapType(m), 6, false));
    return;
    }
    if(membername.equals( "d4"))
    {
    obj.set_d4(DataTypeUtil.verifyArrayLength1(MessageElementUtil.<Integer,double[]>unpackMapType(m), 6, true));
    return;
    }
    if(membername.equals( "d5"))
    {
    obj.set_d5(DataTypeUtil.verifyArrayLength2(MessageElementUtil.<MultiDimArray>unpackListType(m),9,new int[] {3,3}));
    return;
    }
    if(membername.equals( "d6"))
    {
    obj.set_d6(DataTypeUtil.verifyArrayLength2(MessageElementUtil.<Integer,MultiDimArray>unpackMapType(m),9,new int[] {3,3}));
    return;
    }
    if(membername.equals( "testnamedarray1"))
    {
    obj.set_testnamedarray1(MessageElementUtil.<vector3>unpackNamedArrayFromArray(m));
    return;
    }
    if(membername.equals( "testnamedarray2"))
    {
    obj.set_testnamedarray2(MessageElementUtil.<transform>unpackNamedArrayFromArray(m));
    return;
    }
    if(membername.equals( "testnamedarray3"))
    {
    obj.set_testnamedarray3(DataTypeUtil.verifyArrayLength(MessageElementUtil.<transform>unpackNamedArray(m), 10, true));
    return;
    }
    if(membername.equals( "testnamedarray4"))
    {
    obj.set_testnamedarray4(MessageElementUtil.unpackNamedMultiDimArray(m));
    return;
    }
    if(membername.equals( "testnamedarray5"))
    {
    obj.set_testnamedarray5(DataTypeUtil.verifyArrayLength(MessageElementUtil.unpackNamedMultiDimArray(m),6,new int[] {3,2}));
    return;
    }
    if(membername.equals( "c1"))
    {
    obj.set_c1((MessageElementUtil.<CDouble[]>castDataAndDispose(m))[0]);
    return;
    }
    if(membername.equals( "c2"))
    {
    obj.set_c2(MessageElementUtil.<CDouble[]>unpackArray(m));
    return;
    }
    if(membername.equals( "c3"))
    {
    obj.set_c3(MessageElementUtil.unpackMultiDimArray(m));
    return;
    }
    if(membername.equals( "c4"))
    {
    obj.set_c4(MessageElementUtil.<CDouble[]>unpackListType(m));
    return;
    }
    if(membername.equals( "c5"))
    {
    obj.set_c5(MessageElementUtil.<CDouble[]>unpackListType(m));
    return;
    }
    if(membername.equals( "c6"))
    {
    obj.set_c6(MessageElementUtil.<MultiDimArray>unpackListType(m));
    return;
    }
    if(membername.equals( "c7"))
    {
    obj.set_c7((MessageElementUtil.<CSingle[]>castDataAndDispose(m))[0]);
    return;
    }
    if(membername.equals( "c8"))
    {
    obj.set_c8(MessageElementUtil.<CSingle[]>unpackArray(m));
    return;
    }
    if(membername.equals( "c9"))
    {
    obj.set_c9(MessageElementUtil.unpackMultiDimArray(m));
    return;
    }
    if(membername.equals( "c10"))
    {
    obj.set_c10(MessageElementUtil.<CSingle[]>unpackListType(m));
    return;
    }
    if(membername.equals( "c11"))
    {
    obj.set_c11(MessageElementUtil.<CSingle[]>unpackListType(m));
    return;
    }
    if(membername.equals( "c12"))
    {
    obj.set_c12(MessageElementUtil.<MultiDimArray>unpackListType(m));
    return;
    }
    if(membername.equals( "b1"))
    {
    obj.set_b1((MessageElementUtil.<boolean[]>castDataAndDispose(m))[0]);
    return;
    }
    if(membername.equals( "b2"))
    {
    obj.set_b2(MessageElementUtil.<boolean[]>unpackArray(m));
    return;
    }
    if(membername.equals( "b3"))
    {
    obj.set_b3(MessageElementUtil.unpackMultiDimArray(m));
    return;
    }
    if(membername.equals( "b4"))
    {
    obj.set_b4(MessageElementUtil.<boolean[]>unpackListType(m));
    return;
    }
    if(membername.equals( "b5"))
    {
    obj.set_b5(MessageElementUtil.<boolean[]>unpackListType(m));
    return;
    }
    if(membername.equals( "b6"))
    {
    obj.set_b6(MessageElementUtil.<MultiDimArray>unpackListType(m));
    return;
    }
    throw new MemberNotFoundException("Member not found");
    }
    public MessageElement callFunction(String rr_membername, vectorptr_messageelement rr_m) {
    if(rr_membername.equals( "testpod1_func1"))
    {
    testpod1 s=MessageElementUtil.<testpod1>unpackPodFromArray(vectorptr_messageelement_util.findElement(rr_m,"s"));
    this.obj.testpod1_func1(s);
    return new MessageElement("return",new int[] {0});
    }
    if(rr_membername.equals( "testpod1_func2"))
    {
    testpod1 rr_ret=obj.testpod1_func2();
    return MessageElementUtil.<testpod1>packPodToArray("return",rr_ret);
    }
    if(rr_membername.equals( "gen_func1"))
    {
    Generator2<double[]> rr_ret=this.obj.gen_func1();
    int generator_index = innerskel.registerGeneratorServer("gen_func1", new WrappedGenerator2ServerDirectorJava<double[]>(rr_ret));
    return new MessageElement("index",generator_index);
    }
    if(rr_membername.equals( "gen_func2"))
    {
    String name=MessageElementUtil.unpackString(vectorptr_messageelement_util.findElement(rr_m,"name"));
    Generator2<UnsignedBytes> rr_ret=this.obj.gen_func2(name);
    int generator_index = innerskel.registerGeneratorServer("gen_func2", new WrappedGenerator2ServerDirectorJava<UnsignedBytes>(rr_ret));
    return new MessageElement("index",generator_index);
    }
    if(rr_membername.equals( "gen_func3"))
    {
    String name=MessageElementUtil.unpackString(vectorptr_messageelement_util.findElement(rr_m,"name"));
    Generator3<UnsignedBytes> rr_ret=this.obj.gen_func3(name);
    int generator_index = innerskel.registerGeneratorServer("gen_func3", new WrappedGenerator3ServerDirectorJava<UnsignedBytes>(rr_ret));
    return new MessageElement("index",generator_index);
    }
    if(rr_membername.equals( "gen_func4"))
    {
    Generator1<UnsignedBytes,UnsignedBytes> rr_ret=this.obj.gen_func4();
    int generator_index = innerskel.registerGeneratorServer("gen_func4", new WrappedGenerator1ServerDirectorJava<UnsignedBytes,UnsignedBytes>(rr_ret));
    return new MessageElement("index",generator_index);
    }
    if(rr_membername.equals( "gen_func5"))
    {
    Generator1<com.robotraconteur.testing.TestService1.teststruct2,com.robotraconteur.testing.TestService1.teststruct2> rr_ret=this.obj.gen_func5();
    int generator_index = innerskel.registerGeneratorServer("gen_func5", new WrappedGenerator1ServerDirectorJava<com.robotraconteur.testing.TestService1.teststruct2,com.robotraconteur.testing.TestService1.teststruct2>(rr_ret));
    return new MessageElement("index",generator_index);
    }
    throw new MemberNotFoundException("Member not found");
    }
    public Object getSubObj(String name, String ind) {
    if(name.equals( "o4")) {
    return obj.get_o4();
    }
    if(name.equals( "nolock_test")) {
    return obj.get_nolock_test();
    }
    throw new MemberNotFoundException("");
    }
    public void registerEvents(Object obj1) {
    obj=(testroot3)obj1;
    }
    public void unregisterEvents(Object obj1) {
    obj=(testroot3)obj1;
    }
    public Object getCallbackFunction(long endpoint, String membername) {
    throw new MemberNotFoundException("Member not found");
    }
    public  void initPipeServers(Object obj1) {
    obj=(testroot3)obj1;
    obj.set_unreliable1(new Pipe<int[]>(innerskel.getPipe("unreliable1")));
    obj.set_unreliable2(new Pipe<int[]>(innerskel.getPipe("unreliable2")));
    obj.set_p1(new Pipe<int[]>(innerskel.getPipe("p1")));
    obj.set_p2(new Pipe<int[]>(innerskel.getPipe("p2")));
    obj.set_p3(new Pipe<MultiDimArray>(innerskel.getPipe("p3")));
    }
    public void initCallbackServers(Object obj1) {
    obj=(testroot3)obj1;
    }
    public void initWireServers(Object obj1) {
    obj=(testroot3)obj1;
    obj.set_peekwire(new Wire<int[]>(innerskel.getWire("peekwire")));
    obj.set_pokewire(new Wire<int[]>(innerskel.getWire("pokewire")));
    obj.set_w1(new Wire<int[]>(innerskel.getWire("w1")));
    obj.set_w2(new Wire<int[]>(innerskel.getWire("w2")));
    obj.set_w3(new Wire<MultiDimArray>(innerskel.getWire("w3")));
    }
    public WrappedArrayMemoryDirector getArrayMemory(String name) {
    if(name.equals( "readmem")) {
    WrappedArrayMemoryDirectorJava<double[]> dir=new  WrappedArrayMemoryDirectorJava<double[]>(obj.get_readmem());
    return dir;
    }
    if(name.equals( "c_m1")) {
    WrappedArrayMemoryDirectorJava<CDouble[]> dir=new  WrappedArrayMemoryDirectorJava<CDouble[]>(obj.get_c_m1());
    return dir;
    }
    if(name.equals( "c_m3")) {
    WrappedArrayMemoryDirectorJava<CDouble[]> dir=new  WrappedArrayMemoryDirectorJava<CDouble[]>(obj.get_c_m3());
    return dir;
    }
    if(name.equals( "c_m5")) {
    WrappedArrayMemoryDirectorJava<boolean[]> dir=new  WrappedArrayMemoryDirectorJava<boolean[]>(obj.get_c_m5());
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedMultiDimArrayMemoryDirector getMultiDimArrayMemory(String name) {
    if(name.equals( "c_m2")) {
    WrappedMultiDimArrayMemoryDirectorJava<CDouble[]> dir=new  WrappedMultiDimArrayMemoryDirectorJava<CDouble[]>(obj.get_c_m2());
    return dir;
    }
    if(name.equals( "c_m4")) {
    WrappedMultiDimArrayMemoryDirectorJava<CDouble[]> dir=new  WrappedMultiDimArrayMemoryDirectorJava<CDouble[]>(obj.get_c_m4());
    return dir;
    }
    if(name.equals( "c_m6")) {
    WrappedMultiDimArrayMemoryDirectorJava<boolean[]> dir=new  WrappedMultiDimArrayMemoryDirectorJava<boolean[]>(obj.get_c_m6());
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedPodArrayMemoryDirector getPodArrayMemory(String name) {
    if(name.equals( "pod_m1")) {
    WrappedPodArrayMemoryDirectorJava<testpod2[]> dir=new  WrappedPodArrayMemoryDirectorJava<testpod2[]>(obj.get_pod_m1(), testpod2.class);
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedPodMultiDimArrayMemoryDirector getPodMultiDimArrayMemory(String name) {
    if(name.equals( "pod_m2")) {
    WrappedPodMultiDimArrayMemoryDirectorJava<testpod2[]> dir=new  WrappedPodMultiDimArrayMemoryDirectorJava<testpod2[]>(obj.get_pod_m2(), testpod2.class);
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedNamedArrayMemoryDirector getNamedArrayMemory(String name) {
    if(name.equals( "namedarray_m1")) {
    WrappedNamedArrayMemoryDirectorJava<transform[]> dir=new  WrappedNamedArrayMemoryDirectorJava<transform[]>(obj.get_namedarray_m1(), transform.class);
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedNamedMultiDimArrayMemoryDirector getNamedMultiDimArrayMemory(String name) {
    if(name.equals( "namedarray_m2")) {
    WrappedNamedMultiDimArrayMemoryDirectorJava<transform[]> dir=new  WrappedNamedMultiDimArrayMemoryDirectorJava<transform[]>(obj.get_namedarray_m2(), transform.class);
    return dir;
    }
    throw new MemberNotFoundException("Member Not Found");
    }
    public String getRRType() { return "com.robotraconteur.testing.TestService3.testroot3"; }
}

//This file is automatically generated. DO NOT EDIT!
package com.robotraconteur.testing.TestService1;
import java.util.*;
import com.robotraconteur.*;
public class sub1_skel extends ServiceSkel {
    protected sub1 obj;
    public sub1_skel(Object o) { 
    super(o);
    obj=(sub1)o;
    }
    public void releaseCastObject() { 
    obj=null;
    super.releaseCastObject();
    }
    public MessageElement callGetProperty(String membername) {
    if(membername.equals( "d1"))
    {
    double[] ret=obj.get_d1();
    return MessageElementUtil.<double[]>packArray("return",ret);
    }
    if(membername.equals( "d2"))
    {
    MultiDimArray ret=obj.get_d2();
    return MessageElementUtil.packMultiDimArray("return",(MultiDimArray)ret);
    }
    if(membername.equals( "s_ind"))
    {
    String ret=obj.get_s_ind();
    return MessageElementUtil.packString("return",ret);
    }
    if(membername.equals( "i_ind"))
    {
    int ret=obj.get_i_ind();
    return MessageElementUtil.<int[]>packArray("return",new int[] {ret});
    }
    throw new MemberNotFoundException("Member not found");
    }
    public void callSetProperty(String membername, MessageElement m) {
    if(membername.equals( "d1"))
    {
    obj.set_d1(MessageElementUtil.<double[]>unpackArray(m));
    return;
    }
    if(membername.equals( "d2"))
    {
    obj.set_d2(MessageElementUtil.unpackMultiDimArray(m));
    return;
    }
    if(membername.equals( "s_ind"))
    {
    obj.set_s_ind(MessageElementUtil.unpackString(m));
    return;
    }
    if(membername.equals( "i_ind"))
    {
    obj.set_i_ind((MessageElementUtil.<int[]>castDataAndDispose(m))[0]);
    return;
    }
    throw new MemberNotFoundException("Member not found");
    }
    public MessageElement callFunction(String rr_membername, vectorptr_messageelement rr_m) {
    throw new MemberNotFoundException("Member not found");
    }
    public Object getSubObj(String name, String ind) {
    if(name.equals( "o2_1")) {
    return obj.get_o2_1();
    }
    if(name.equals( "o2_2")) {
    return obj.get_o2_2(Integer.valueOf(ind));
    }
    if(name.equals( "o2_3")) {
    return obj.get_o2_3(ind);
    }
    throw new MemberNotFoundException("");
    }
    public void registerEvents(Object obj1) {
    obj=(sub1)obj1;
    }
    public void unregisterEvents(Object obj1) {
    obj=(sub1)obj1;
    }
    public Object getCallbackFunction(long endpoint, String membername) {
    throw new MemberNotFoundException("Member not found");
    }
    public  void initPipeServers(Object obj1) {
    obj=(sub1)obj1;
    }
    public void initCallbackServers(Object obj1) {
    obj=(sub1)obj1;
    }
    public void initWireServers(Object obj1) {
    obj=(sub1)obj1;
    }
    public WrappedArrayMemoryDirector getArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedMultiDimArrayMemoryDirector getMultiDimArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedPodArrayMemoryDirector getPodArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedPodMultiDimArrayMemoryDirector getPodMultiDimArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedNamedArrayMemoryDirector getNamedArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public WrappedNamedMultiDimArrayMemoryDirector getNamedMultiDimArrayMemory(String name) {
    throw new MemberNotFoundException("Member Not Found");
    }
    public String getRRType() { return "com.robotraconteur.testing.TestService1.sub1"; }
}

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class MemberDefinitionUtil {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected MemberDefinitionUtil(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(MemberDefinitionUtil obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_MemberDefinitionUtil(swigCPtr);
      }
      swigCPtr = 0;
    }
  }


public static MemberDefinition swigCast(MemberDefinition i) 
{
	MemberDefinition o=null;
	o=toProperty(i);
	if (o!=null) return (PropertyDefinition)o;
	o=toFunction(i);
	if (o!=null) return (FunctionDefinition)o;
	o=toEvent(i);
	if (o!=null) return (EventDefinition)o;
	o=toObjRef(i);
	if (o!=null) return (ObjRefDefinition)o;
	o=toPipe(i);
	if (o!=null) return (PipeDefinition)o;
	o=toCallback(i);
	if (o!=null) return (CallbackDefinition)o;
	o=toWire(i);
	if (o!=null) return (WireDefinition)o;
	o=toMemory(i);
	if (o!=null) return (MemoryDefinition)o;
	throw new RuntimeException (new Exception("Invalid MemberDefinition"));
	
}


  public static PropertyDefinition toProperty(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toProperty(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new PropertyDefinition(cPtr, true);
  }

  public static FunctionDefinition toFunction(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toFunction(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new FunctionDefinition(cPtr, true);
  }

  public static ObjRefDefinition toObjRef(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toObjRef(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new ObjRefDefinition(cPtr, true);
  }

  public static EventDefinition toEvent(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toEvent(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new EventDefinition(cPtr, true);
  }

  public static PipeDefinition toPipe(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toPipe(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new PipeDefinition(cPtr, true);
  }

  public static CallbackDefinition toCallback(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toCallback(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new CallbackDefinition(cPtr, true);
  }

  public static WireDefinition toWire(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toWire(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new WireDefinition(cPtr, true);
  }

  public static MemoryDefinition toMemory(MemberDefinition t) {
    long cPtr = RobotRaconteurJavaJNI.MemberDefinitionUtil_toMemory(MemberDefinition.getCPtr(t), t);
    return (cPtr == 0) ? null : new MemoryDefinition(cPtr, true);
  }

  public MemberDefinitionUtil() {
    this(RobotRaconteurJavaJNI.new_MemberDefinitionUtil(), true);
  }

}
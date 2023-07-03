/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WireDefinition extends MemberDefinition {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected WireDefinition(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.WireDefinition_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WireDefinition obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void swigSetCMemOwn(boolean own) {
    swigCMemOwnDerived = own;
    super.swigSetCMemOwn(own);
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwnDerived) {
        swigCMemOwnDerived = false;
        RobotRaconteurJavaJNI.delete_WireDefinition(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public void setType(TypeDefinition value) {
    RobotRaconteurJavaJNI.WireDefinition_Type_set(swigCPtr, this, TypeDefinition.getCPtr(value), value);
  }

  public TypeDefinition getType() {
    long cPtr = RobotRaconteurJavaJNI.WireDefinition_Type_get(swigCPtr, this);
    return (cPtr == 0) ? null : new TypeDefinition(cPtr, true);
  }

  public WireDefinition(ServiceEntryDefinition ServiceEntry) {
    this(RobotRaconteurJavaJNI.new_WireDefinition(ServiceEntryDefinition.getCPtr(ServiceEntry), ServiceEntry), true);
  }

  public String toString() {
    return RobotRaconteurJavaJNI.WireDefinition_toString(swigCPtr, this);
  }

  public void fromString(String s, ServiceDefinitionParseInfo parse_info) {
    RobotRaconteurJavaJNI.WireDefinition_fromString__SWIG_0(swigCPtr, this, s, ServiceDefinitionParseInfo.getCPtr(parse_info), parse_info);
  }

  public void fromString(String s) {
    RobotRaconteurJavaJNI.WireDefinition_fromString__SWIG_1(swigCPtr, this, s);
  }

}

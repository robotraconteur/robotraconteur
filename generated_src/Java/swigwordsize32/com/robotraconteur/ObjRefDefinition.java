/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class ObjRefDefinition extends MemberDefinition {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected ObjRefDefinition(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.ObjRefDefinition_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ObjRefDefinition obj) {
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
        RobotRaconteurJavaJNI.delete_ObjRefDefinition(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public void setObjectType(String value) {
    RobotRaconteurJavaJNI.ObjRefDefinition_ObjectType_set(swigCPtr, this, value);
  }

  public String getObjectType() {
    return RobotRaconteurJavaJNI.ObjRefDefinition_ObjectType_get(swigCPtr, this);
  }

  public void setArrayType(DataTypes_ArrayTypes value) {
    RobotRaconteurJavaJNI.ObjRefDefinition_ArrayType_set(swigCPtr, this, value.swigValue());
  }

  public DataTypes_ArrayTypes getArrayType() {
    return DataTypes_ArrayTypes.swigToEnum(RobotRaconteurJavaJNI.ObjRefDefinition_ArrayType_get(swigCPtr, this));
  }

  public void setContainerType(DataTypes_ContainerTypes value) {
    RobotRaconteurJavaJNI.ObjRefDefinition_ContainerType_set(swigCPtr, this, value.swigValue());
  }

  public DataTypes_ContainerTypes getContainerType() {
    return DataTypes_ContainerTypes.swigToEnum(RobotRaconteurJavaJNI.ObjRefDefinition_ContainerType_get(swigCPtr, this));
  }

  public ObjRefDefinition(ServiceEntryDefinition ServiceEntry) {
    this(RobotRaconteurJavaJNI.new_ObjRefDefinition(ServiceEntryDefinition.getCPtr(ServiceEntry), ServiceEntry), true);
  }

  public String toString() {
    return RobotRaconteurJavaJNI.ObjRefDefinition_toString(swigCPtr, this);
  }

  public void fromString(String s, ServiceDefinitionParseInfo parse_info) {
    RobotRaconteurJavaJNI.ObjRefDefinition_fromString__SWIG_0(swigCPtr, this, s, ServiceDefinitionParseInfo.getCPtr(parse_info), parse_info);
  }

  public void fromString(String s) {
    RobotRaconteurJavaJNI.ObjRefDefinition_fromString__SWIG_1(swigCPtr, this, s);
  }

}
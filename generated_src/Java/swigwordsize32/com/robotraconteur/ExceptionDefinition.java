/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class ExceptionDefinition {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected ExceptionDefinition(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ExceptionDefinition obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void swigSetCMemOwn(boolean own) {
    swigCMemOwn = own;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_ExceptionDefinition(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setName(String value) {
    RobotRaconteurJavaJNI.ExceptionDefinition_Name_set(swigCPtr, this, value);
  }

  public String getName() {
    return RobotRaconteurJavaJNI.ExceptionDefinition_Name_get(swigCPtr, this);
  }

  public void setDocString(String value) {
    RobotRaconteurJavaJNI.ExceptionDefinition_DocString_set(swigCPtr, this, value);
  }

  public String getDocString() {
    return RobotRaconteurJavaJNI.ExceptionDefinition_DocString_get(swigCPtr, this);
  }

  public void setParseInfo(ServiceDefinitionParseInfo value) {
    RobotRaconteurJavaJNI.ExceptionDefinition_ParseInfo_set(swigCPtr, this, ServiceDefinitionParseInfo.getCPtr(value), value);
  }

  public ServiceDefinitionParseInfo getParseInfo() {
    long cPtr = RobotRaconteurJavaJNI.ExceptionDefinition_ParseInfo_get(swigCPtr, this);
    return (cPtr == 0) ? null : new ServiceDefinitionParseInfo(cPtr, false);
  }

  public ServiceDefinition getService() {
    long cPtr = RobotRaconteurJavaJNI.ExceptionDefinition_getService(swigCPtr, this);
    return (cPtr == 0) ? null : new ServiceDefinition(cPtr, true);
  }

  public void setService(ServiceDefinition value) {
    RobotRaconteurJavaJNI.ExceptionDefinition_setService(swigCPtr, this, ServiceDefinition.getCPtr(value), value);
  }

  public ExceptionDefinition(ServiceDefinition service) {
    this(RobotRaconteurJavaJNI.new_ExceptionDefinition(ServiceDefinition.getCPtr(service), service), true);
  }

  public String toString() {
    return RobotRaconteurJavaJNI.ExceptionDefinition_toString(swigCPtr, this);
  }

  public void fromString(String s, ServiceDefinitionParseInfo parse_info) {
    RobotRaconteurJavaJNI.ExceptionDefinition_fromString__SWIG_0(swigCPtr, this, s, ServiceDefinitionParseInfo.getCPtr(parse_info), parse_info);
  }

  public void fromString(String s) {
    RobotRaconteurJavaJNI.ExceptionDefinition_fromString__SWIG_1(swigCPtr, this, s);
  }

  public void reset() {
    RobotRaconteurJavaJNI.ExceptionDefinition_reset(swigCPtr, this);
  }

}

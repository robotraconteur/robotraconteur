/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedGeneratorClient_TryGetNextResult {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedGeneratorClient_TryGetNextResult(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedGeneratorClient_TryGetNextResult obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedGeneratorClient_TryGetNextResult(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setValue(MessageElement value) {
    RobotRaconteurJavaJNI.WrappedGeneratorClient_TryGetNextResult_value_set(swigCPtr, this, MessageElement.getCPtr(value), value);
  }

  public MessageElement getValue() {
    long cPtr = RobotRaconteurJavaJNI.WrappedGeneratorClient_TryGetNextResult_value_get(swigCPtr, this);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

  public void setRes(boolean value) {
    RobotRaconteurJavaJNI.WrappedGeneratorClient_TryGetNextResult_res_set(swigCPtr, this, value);
  }

  public boolean getRes() {
    return RobotRaconteurJavaJNI.WrappedGeneratorClient_TryGetNextResult_res_get(swigCPtr, this);
  }

}

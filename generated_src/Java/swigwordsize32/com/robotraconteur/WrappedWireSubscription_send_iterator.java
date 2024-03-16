/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedWireSubscription_send_iterator {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedWireSubscription_send_iterator(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedWireSubscription_send_iterator obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(WrappedWireSubscription_send_iterator obj) {
    long ptr = 0;
    if (obj != null) {
      if (!obj.swigCMemOwn)
        throw new RuntimeException("Cannot release ownership as memory is not owned");
      ptr = obj.swigCPtr;
      obj.swigCMemOwn = false;
      obj.delete();
    }
    return ptr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_WrappedWireSubscription_send_iterator(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public WrappedWireSubscription_send_iterator(WrappedWireSubscription sub) {
    this(RobotRaconteurJavaJNI.new_WrappedWireSubscription_send_iterator(WrappedWireSubscription.getCPtr(sub), sub), true);
  }

  public WrappedWireConnection next() {
    long cPtr = RobotRaconteurJavaJNI.WrappedWireSubscription_send_iterator_next(swigCPtr, this);
    return (cPtr == 0) ? null : new WrappedWireConnection(cPtr, true);
  }

  public void setOutValue(MessageElement value) {
    RobotRaconteurJavaJNI.WrappedWireSubscription_send_iterator_setOutValue(swigCPtr, this, MessageElement.getCPtr(value), value);
  }

  public TypeDefinition getType() {
    long cPtr = RobotRaconteurJavaJNI.WrappedWireSubscription_send_iterator_getType(swigCPtr, this);
    return (cPtr == 0) ? null : new TypeDefinition(cPtr, true);
  }

  public WrappedServiceStub getStub() {
    long cPtr = RobotRaconteurJavaJNI.WrappedWireSubscription_send_iterator_getStub(swigCPtr, this);
    return (cPtr == 0) ? null : new WrappedServiceStub(cPtr, true);
  }

}
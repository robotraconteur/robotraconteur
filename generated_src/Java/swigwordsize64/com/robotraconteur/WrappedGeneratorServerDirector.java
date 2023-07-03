/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedGeneratorServerDirector {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedGeneratorServerDirector(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedGeneratorServerDirector obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(WrappedGeneratorServerDirector obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedGeneratorServerDirector(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  protected void swigDirectorDisconnect() {
    swigCMemOwn = false;
    delete();
  }

  public void swigReleaseOwnership() {
    swigCMemOwn = false;
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_change_ownership(this, swigCPtr, false);
  }

  public void swigTakeOwnership() {
    swigCMemOwn = true;
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_change_ownership(this, swigCPtr, true);
  }

  public WrappedGeneratorServerDirector() {
    this(RobotRaconteurJavaJNI.new_WrappedGeneratorServerDirector(), true);
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_director_connect(this, swigCPtr, true, true);
  }

  public MessageElement next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adaptor) {
    long cPtr = RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_next(swigCPtr, this, MessageElement.getCPtr(m), m, WrappedServiceSkelAsyncAdapter.getCPtr(async_adaptor), async_adaptor);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

  public void abort(WrappedServiceSkelAsyncAdapter async_adapter) {
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_abort(swigCPtr, this, WrappedServiceSkelAsyncAdapter.getCPtr(async_adapter), async_adapter);
  }

  public void close(WrappedServiceSkelAsyncAdapter async_adapter) {
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_close(swigCPtr, this, WrappedServiceSkelAsyncAdapter.getCPtr(async_adapter), async_adapter);
  }

  public void setObjectheapid(int value) {
    RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_objectheapid_set(swigCPtr, this, value);
  }

  public int getObjectheapid() {
    return RobotRaconteurJavaJNI.WrappedGeneratorServerDirector_objectheapid_get(swigCPtr, this);
  }

}

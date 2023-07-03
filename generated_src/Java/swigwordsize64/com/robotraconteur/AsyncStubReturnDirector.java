/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class AsyncStubReturnDirector {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected AsyncStubReturnDirector(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(AsyncStubReturnDirector obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(AsyncStubReturnDirector obj) {
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
        RobotRaconteurJavaJNI.delete_AsyncStubReturnDirector(swigCPtr);
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
    RobotRaconteurJavaJNI.AsyncStubReturnDirector_change_ownership(this, swigCPtr, false);
  }

  public void swigTakeOwnership() {
    swigCMemOwn = true;
    RobotRaconteurJavaJNI.AsyncStubReturnDirector_change_ownership(this, swigCPtr, true);
  }

  public void handler(WrappedServiceStub stub, HandlerErrorInfo error) {
    RobotRaconteurJavaJNI.AsyncStubReturnDirector_handler(swigCPtr, this, WrappedServiceStub.getCPtr(stub), stub, HandlerErrorInfo.getCPtr(error), error);
  }

  public AsyncStubReturnDirector() {
    this(RobotRaconteurJavaJNI.new_AsyncStubReturnDirector(), true);
    RobotRaconteurJavaJNI.AsyncStubReturnDirector_director_connect(this, swigCPtr, true, true);
  }

}

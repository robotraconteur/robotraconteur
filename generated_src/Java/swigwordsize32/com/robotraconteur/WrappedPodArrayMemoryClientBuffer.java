/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedPodArrayMemoryClientBuffer {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedPodArrayMemoryClientBuffer(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedPodArrayMemoryClientBuffer obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedPodArrayMemoryClientBuffer(swigCPtr);
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
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_change_ownership(this, swigCPtr, false);
  }

  public void swigTakeOwnership() {
    swigCMemOwn = true;
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_change_ownership(this, swigCPtr, true);
  }

  public void unpackReadResult(MessageElementNestedElementList res, java.math.BigInteger bufferpos, java.math.BigInteger count) {
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_unpackReadResult(swigCPtr, this, MessageElementNestedElementList.getCPtr(res), res, bufferpos, count);
  }

  public MessageElementNestedElementList packWriteRequest(java.math.BigInteger bufferpos, java.math.BigInteger count) {
    long cPtr = RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_packWriteRequest(swigCPtr, this, bufferpos, count);
    return (cPtr == 0) ? null : new MessageElementNestedElementList(cPtr, true);
  }

  public java.math.BigInteger getBufferLength() {
    return RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_getBufferLength(swigCPtr, this);
  }

  public WrappedPodArrayMemoryClientBuffer() {
    this(RobotRaconteurJavaJNI.new_WrappedPodArrayMemoryClientBuffer(), true);
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClientBuffer_director_connect(this, swigCPtr, true, true);
  }

}
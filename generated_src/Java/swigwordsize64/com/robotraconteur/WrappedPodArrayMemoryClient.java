/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedPodArrayMemoryClient {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected WrappedPodArrayMemoryClient(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedPodArrayMemoryClient obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedPodArrayMemoryClient(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public long length() {
    return RobotRaconteurJavaJNI.WrappedPodArrayMemoryClient_length(swigCPtr, this);
  }

  public MemberDefinition_Direction direction() {
    return MemberDefinition_Direction.swigToEnum(RobotRaconteurJavaJNI.WrappedPodArrayMemoryClient_direction(swigCPtr, this));
  }

  public void read(long memorypos, WrappedPodArrayMemoryClientBuffer buffer, long bufferpos, long count) {
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClient_read(swigCPtr, this, memorypos, WrappedPodArrayMemoryClientBuffer.getCPtr(buffer), buffer, bufferpos, count);
  }

  public void write(long memorypos, WrappedPodArrayMemoryClientBuffer buffer, long bufferpos, long count) {
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryClient_write(swigCPtr, this, memorypos, WrappedPodArrayMemoryClientBuffer.getCPtr(buffer), buffer, bufferpos, count);
  }

}

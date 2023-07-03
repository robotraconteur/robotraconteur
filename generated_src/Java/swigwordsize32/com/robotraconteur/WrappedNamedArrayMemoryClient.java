/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedNamedArrayMemoryClient {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected WrappedNamedArrayMemoryClient(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedNamedArrayMemoryClient obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedNamedArrayMemoryClient(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public long length() {
    return RobotRaconteurJavaJNI.WrappedNamedArrayMemoryClient_length(swigCPtr, this);
  }

  public MemberDefinition_Direction direction() {
    return MemberDefinition_Direction.swigToEnum(RobotRaconteurJavaJNI.WrappedNamedArrayMemoryClient_direction(swigCPtr, this));
  }

  public void read(long memorypos, WrappedNamedArrayMemoryClientBuffer buffer, long bufferpos, long count) {
    RobotRaconteurJavaJNI.WrappedNamedArrayMemoryClient_read(swigCPtr, this, memorypos, WrappedNamedArrayMemoryClientBuffer.getCPtr(buffer), buffer, bufferpos, count);
  }

  public void write(long memorypos, WrappedNamedArrayMemoryClientBuffer buffer, long bufferpos, long count) {
    RobotRaconteurJavaJNI.WrappedNamedArrayMemoryClient_write(swigCPtr, this, memorypos, WrappedNamedArrayMemoryClientBuffer.getCPtr(buffer), buffer, bufferpos, count);
  }

}

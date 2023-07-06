/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedPodMultiDimArrayMemoryClient {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected WrappedPodMultiDimArrayMemoryClient(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedPodMultiDimArrayMemoryClient obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedPodMultiDimArrayMemoryClient(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public vector_uint64_t dimensions() {
    return new vector_uint64_t(RobotRaconteurJavaJNI.WrappedPodMultiDimArrayMemoryClient_dimensions(swigCPtr, this), true);
  }

  public java.math.BigInteger dimCount() {
    return RobotRaconteurJavaJNI.WrappedPodMultiDimArrayMemoryClient_dimCount(swigCPtr, this);
  }

  public MemberDefinition_Direction direction() {
    return MemberDefinition_Direction.swigToEnum(RobotRaconteurJavaJNI.WrappedPodMultiDimArrayMemoryClient_direction(swigCPtr, this));
  }

  public void read(vector_uint64_t memorypos, WrappedPodMultiDimArrayMemoryClientBuffer buffer, vector_uint64_t bufferpos, vector_uint64_t count) {
    RobotRaconteurJavaJNI.WrappedPodMultiDimArrayMemoryClient_read(swigCPtr, this, vector_uint64_t.getCPtr(memorypos), memorypos, WrappedPodMultiDimArrayMemoryClientBuffer.getCPtr(buffer), buffer, vector_uint64_t.getCPtr(bufferpos), bufferpos, vector_uint64_t.getCPtr(count), count);
  }

  public void write(vector_uint64_t memorypos, WrappedPodMultiDimArrayMemoryClientBuffer buffer, vector_uint64_t bufferpos, vector_uint64_t count) {
    RobotRaconteurJavaJNI.WrappedPodMultiDimArrayMemoryClient_write(swigCPtr, this, vector_uint64_t.getCPtr(memorypos), memorypos, WrappedPodMultiDimArrayMemoryClientBuffer.getCPtr(buffer), buffer, vector_uint64_t.getCPtr(bufferpos), bufferpos, vector_uint64_t.getCPtr(count), count);
  }

}

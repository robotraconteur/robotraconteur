/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedMultiDimArrayMemoryParams {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedMultiDimArrayMemoryParams(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedMultiDimArrayMemoryParams obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(WrappedMultiDimArrayMemoryParams obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedMultiDimArrayMemoryParams(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setMemorypos(vector_uint64_t value) {
    RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_memorypos_set(swigCPtr, this, vector_uint64_t.getCPtr(value), value);
  }

  public vector_uint64_t getMemorypos() {
    long cPtr = RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_memorypos_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vector_uint64_t(cPtr, false);
  }

  public void setBuffer(RRMultiDimArrayUntyped value) {
    RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_buffer_set(swigCPtr, this, RRMultiDimArrayUntyped.getCPtr(value), value);
  }

  public RRMultiDimArrayUntyped getBuffer() {
    long cPtr = RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_buffer_get(swigCPtr, this);
    return (cPtr == 0) ? null : new RRMultiDimArrayUntyped(cPtr, true);
  }

  public void setBufferpos(vector_uint64_t value) {
    RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_bufferpos_set(swigCPtr, this, vector_uint64_t.getCPtr(value), value);
  }

  public vector_uint64_t getBufferpos() {
    long cPtr = RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_bufferpos_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vector_uint64_t(cPtr, false);
  }

  public void setCount(vector_uint64_t value) {
    RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_count_set(swigCPtr, this, vector_uint64_t.getCPtr(value), value);
  }

  public vector_uint64_t getCount() {
    long cPtr = RobotRaconteurJavaJNI.WrappedMultiDimArrayMemoryParams_count_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vector_uint64_t(cPtr, false);
  }

  public WrappedMultiDimArrayMemoryParams() {
    this(RobotRaconteurJavaJNI.new_WrappedMultiDimArrayMemoryParams(), true);
  }

}

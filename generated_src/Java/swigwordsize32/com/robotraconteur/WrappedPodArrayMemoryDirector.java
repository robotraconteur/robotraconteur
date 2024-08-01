/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedPodArrayMemoryDirector {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedPodArrayMemoryDirector(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedPodArrayMemoryDirector obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedPodArrayMemoryDirector(swigCPtr);
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
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_change_ownership(this, swigCPtr, false);
  }

  public void swigTakeOwnership() {
    swigCMemOwn = true;
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_change_ownership(this, swigCPtr, true);
  }

  public WrappedPodArrayMemoryDirector() {
    this(RobotRaconteurJavaJNI.new_WrappedPodArrayMemoryDirector(), true);
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_director_connect(this, swigCPtr, true, true);
  }

  public long length() {
    return RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_length(swigCPtr, this);
  }

  public MessageElementNestedElementList read(long memorypos, long bufferpos, long count) {
    long cPtr = RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_read(swigCPtr, this, memorypos, bufferpos, count);
    return (cPtr == 0) ? null : new MessageElementNestedElementList(cPtr, true);
  }

  public void write(long memorypos, MessageElementNestedElementList buffer, long bufferpos, long count) {
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_write(swigCPtr, this, memorypos, MessageElementNestedElementList.getCPtr(buffer), buffer, bufferpos, count);
  }

  public void setObjectheapid(int value) {
    RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_objectheapid_set(swigCPtr, this, value);
  }

  public int getObjectheapid() {
    return RobotRaconteurJavaJNI.WrappedPodArrayMemoryDirector_objectheapid_get(swigCPtr, this);
  }

}

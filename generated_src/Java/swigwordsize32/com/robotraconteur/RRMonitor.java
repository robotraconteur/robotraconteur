/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class RRMonitor {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected RRMonitor(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(RRMonitor obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(RRMonitor obj) {
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
        RobotRaconteurJavaJNI.delete_RRMonitor(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void lock() {
    RobotRaconteurJavaJNI.RRMonitor_lock(swigCPtr, this);
  }

  public boolean tryLock(int timeout) {
    return RobotRaconteurJavaJNI.RRMonitor_tryLock(swigCPtr, this, timeout);
  }

  public void unlock() {
    RobotRaconteurJavaJNI.RRMonitor_unlock(swigCPtr, this);
  }

  public RRMonitor() {
    this(RobotRaconteurJavaJNI.new_RRMonitor(), true);
  }

}

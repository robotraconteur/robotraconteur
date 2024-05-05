/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedPipeSubscriptionDirector {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedPipeSubscriptionDirector(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedPipeSubscriptionDirector obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedPipeSubscriptionDirector(swigCPtr);
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
    RobotRaconteurJavaJNI.WrappedPipeSubscriptionDirector_change_ownership(this, swigCPtr, false);
  }

  public void swigTakeOwnership() {
    swigCMemOwn = true;
    RobotRaconteurJavaJNI.WrappedPipeSubscriptionDirector_change_ownership(this, swigCPtr, true);
  }

  public void pipePacketReceived(WrappedPipeSubscription pipe_subscription) {
    RobotRaconteurJavaJNI.WrappedPipeSubscriptionDirector_pipePacketReceived(swigCPtr, this, WrappedPipeSubscription.getCPtr(pipe_subscription), pipe_subscription);
  }

  public WrappedPipeSubscriptionDirector() {
    this(RobotRaconteurJavaJNI.new_WrappedPipeSubscriptionDirector(), true);
    RobotRaconteurJavaJNI.WrappedPipeSubscriptionDirector_director_connect(this, swigCPtr, true, true);
  }

}
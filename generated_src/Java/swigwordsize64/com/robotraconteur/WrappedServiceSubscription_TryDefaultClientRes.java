/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedServiceSubscription_TryDefaultClientRes {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected WrappedServiceSubscription_TryDefaultClientRes(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedServiceSubscription_TryDefaultClientRes obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedServiceSubscription_TryDefaultClientRes(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setRes(boolean value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscription_TryDefaultClientRes_res_set(swigCPtr, this, value);
  }

  public boolean getRes() {
    return RobotRaconteurJavaJNI.WrappedServiceSubscription_TryDefaultClientRes_res_get(swigCPtr, this);
  }

  public void setClient(WrappedServiceStub value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscription_TryDefaultClientRes_client_set(swigCPtr, this, WrappedServiceStub.getCPtr(value), value);
  }

  public WrappedServiceStub getClient() {
    long cPtr = RobotRaconteurJavaJNI.WrappedServiceSubscription_TryDefaultClientRes_client_get(swigCPtr, this);
    return (cPtr == 0) ? null : new WrappedServiceStub(cPtr, true);
  }

  public WrappedServiceSubscription_TryDefaultClientRes() {
    this(RobotRaconteurJavaJNI.new_WrappedServiceSubscription_TryDefaultClientRes(), true);
  }

}

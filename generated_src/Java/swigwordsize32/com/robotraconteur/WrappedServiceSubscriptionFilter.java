/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedServiceSubscriptionFilter {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected WrappedServiceSubscriptionFilter(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedServiceSubscriptionFilter obj) {
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
        RobotRaconteurJavaJNI.delete_WrappedServiceSubscriptionFilter(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setNodes(vectorptr_wrappedservicesubscriptionnode value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_Nodes_set(swigCPtr, this, vectorptr_wrappedservicesubscriptionnode.getCPtr(value), value);
  }

  public vectorptr_wrappedservicesubscriptionnode getNodes() {
    long cPtr = RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_Nodes_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorptr_wrappedservicesubscriptionnode(cPtr, false);
  }

  public void setServiceNames(vectorstring value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_ServiceNames_set(swigCPtr, this, vectorstring.getCPtr(value), value);
  }

  public vectorstring getServiceNames() {
    long cPtr = RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_ServiceNames_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorstring(cPtr, false);
  }

  public void setTransportSchemes(vectorstring value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_TransportSchemes_set(swigCPtr, this, vectorstring.getCPtr(value), value);
  }

  public vectorstring getTransportSchemes() {
    long cPtr = RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_TransportSchemes_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorstring(cPtr, false);
  }

  public void setRRPredicateDirector(WrappedServiceSubscriptionFilterPredicateDirector director, int id) {
    RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_setRRPredicateDirector(swigCPtr, this, WrappedServiceSubscriptionFilterPredicateDirector.getCPtr(director), director, id);
  }

  public void setMaxConnections(long value) {
    RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_MaxConnections_set(swigCPtr, this, value);
  }

  public long getMaxConnections() {
    return RobotRaconteurJavaJNI.WrappedServiceSubscriptionFilter_MaxConnections_get(swigCPtr, this);
  }

  public WrappedServiceSubscriptionFilter() {
    this(RobotRaconteurJavaJNI.new_WrappedServiceSubscriptionFilter(), true);
  }

}

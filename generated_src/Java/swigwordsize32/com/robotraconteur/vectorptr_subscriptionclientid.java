/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class vectorptr_subscriptionclientid extends java.util.AbstractList<WrappedServiceSubscriptionClientID> implements java.util.RandomAccess {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected vectorptr_subscriptionclientid(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(vectorptr_subscriptionclientid obj) {
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
        RobotRaconteurJavaJNI.delete_vectorptr_subscriptionclientid(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public vectorptr_subscriptionclientid(WrappedServiceSubscriptionClientID[] initialElements) {
    this();
    reserve(initialElements.length);

    for (WrappedServiceSubscriptionClientID element : initialElements) {
      add(element);
    }
  }

  public vectorptr_subscriptionclientid(Iterable<WrappedServiceSubscriptionClientID> initialElements) {
    this();
    for (WrappedServiceSubscriptionClientID element : initialElements) {
      add(element);
    }
  }

  public WrappedServiceSubscriptionClientID get(int index) {
    return doGet(index);
  }

  public WrappedServiceSubscriptionClientID set(int index, WrappedServiceSubscriptionClientID e) {
    return doSet(index, e);
  }

  public boolean add(WrappedServiceSubscriptionClientID e) {
    modCount++;
    doAdd(e);
    return true;
  }

  public void add(int index, WrappedServiceSubscriptionClientID e) {
    modCount++;
    doAdd(index, e);
  }

  public WrappedServiceSubscriptionClientID remove(int index) {
    modCount++;
    return doRemove(index);
  }

  protected void removeRange(int fromIndex, int toIndex) {
    modCount++;
    doRemoveRange(fromIndex, toIndex);
  }

  public int size() {
    return doSize();
  }

  public vectorptr_subscriptionclientid() {
    this(RobotRaconteurJavaJNI.new_vectorptr_subscriptionclientid__SWIG_0(), true);
  }

  public vectorptr_subscriptionclientid(vectorptr_subscriptionclientid other) {
    this(RobotRaconteurJavaJNI.new_vectorptr_subscriptionclientid__SWIG_1(vectorptr_subscriptionclientid.getCPtr(other), other), true);
  }

  public long capacity() {
    return RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_isEmpty(swigCPtr, this);
  }

  public void clear() {
    RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_clear(swigCPtr, this);
  }

  public vectorptr_subscriptionclientid(int count, WrappedServiceSubscriptionClientID value) {
    this(RobotRaconteurJavaJNI.new_vectorptr_subscriptionclientid__SWIG_2(count, WrappedServiceSubscriptionClientID.getCPtr(value), value), true);
  }

  private int doSize() {
    return RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doSize(swigCPtr, this);
  }

  private void doAdd(WrappedServiceSubscriptionClientID x) {
    RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doAdd__SWIG_0(swigCPtr, this, WrappedServiceSubscriptionClientID.getCPtr(x), x);
  }

  private void doAdd(int index, WrappedServiceSubscriptionClientID x) {
    RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doAdd__SWIG_1(swigCPtr, this, index, WrappedServiceSubscriptionClientID.getCPtr(x), x);
  }

  private WrappedServiceSubscriptionClientID doRemove(int index) {
    return new WrappedServiceSubscriptionClientID(RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doRemove(swigCPtr, this, index), true);
  }

  private WrappedServiceSubscriptionClientID doGet(int index) {
    return new WrappedServiceSubscriptionClientID(RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doGet(swigCPtr, this, index), false);
  }

  private WrappedServiceSubscriptionClientID doSet(int index, WrappedServiceSubscriptionClientID val) {
    return new WrappedServiceSubscriptionClientID(RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doSet(swigCPtr, this, index, WrappedServiceSubscriptionClientID.getCPtr(val), val), true);
  }

  private void doRemoveRange(int fromIndex, int toIndex) {
    RobotRaconteurJavaJNI.vectorptr_subscriptionclientid_doRemoveRange(swigCPtr, this, fromIndex, toIndex);
  }

}

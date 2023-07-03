/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class vectorptr_wrappedservicesubscriptionnode extends java.util.AbstractList<WrappedServiceSubscriptionFilterNode> implements java.util.RandomAccess {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected vectorptr_wrappedservicesubscriptionnode(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(vectorptr_wrappedservicesubscriptionnode obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(vectorptr_wrappedservicesubscriptionnode obj) {
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
        RobotRaconteurJavaJNI.delete_vectorptr_wrappedservicesubscriptionnode(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public vectorptr_wrappedservicesubscriptionnode(WrappedServiceSubscriptionFilterNode[] initialElements) {
    this();
    reserve(initialElements.length);

    for (WrappedServiceSubscriptionFilterNode element : initialElements) {
      add(element);
    }
  }

  public vectorptr_wrappedservicesubscriptionnode(Iterable<WrappedServiceSubscriptionFilterNode> initialElements) {
    this();
    for (WrappedServiceSubscriptionFilterNode element : initialElements) {
      add(element);
    }
  }

  public WrappedServiceSubscriptionFilterNode get(int index) {
    return doGet(index);
  }

  public WrappedServiceSubscriptionFilterNode set(int index, WrappedServiceSubscriptionFilterNode e) {
    return doSet(index, e);
  }

  public boolean add(WrappedServiceSubscriptionFilterNode e) {
    modCount++;
    doAdd(e);
    return true;
  }

  public void add(int index, WrappedServiceSubscriptionFilterNode e) {
    modCount++;
    doAdd(index, e);
  }

  public WrappedServiceSubscriptionFilterNode remove(int index) {
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

  public vectorptr_wrappedservicesubscriptionnode() {
    this(RobotRaconteurJavaJNI.new_vectorptr_wrappedservicesubscriptionnode__SWIG_0(), true);
  }

  public vectorptr_wrappedservicesubscriptionnode(vectorptr_wrappedservicesubscriptionnode other) {
    this(RobotRaconteurJavaJNI.new_vectorptr_wrappedservicesubscriptionnode__SWIG_1(vectorptr_wrappedservicesubscriptionnode.getCPtr(other), other), true);
  }

  public long capacity() {
    return RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_isEmpty(swigCPtr, this);
  }

  public void clear() {
    RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_clear(swigCPtr, this);
  }

  public vectorptr_wrappedservicesubscriptionnode(int count, WrappedServiceSubscriptionFilterNode value) {
    this(RobotRaconteurJavaJNI.new_vectorptr_wrappedservicesubscriptionnode__SWIG_2(count, WrappedServiceSubscriptionFilterNode.getCPtr(value), value), true);
  }

  private int doSize() {
    return RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doSize(swigCPtr, this);
  }

  private void doAdd(WrappedServiceSubscriptionFilterNode x) {
    RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doAdd__SWIG_0(swigCPtr, this, WrappedServiceSubscriptionFilterNode.getCPtr(x), x);
  }

  private void doAdd(int index, WrappedServiceSubscriptionFilterNode x) {
    RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doAdd__SWIG_1(swigCPtr, this, index, WrappedServiceSubscriptionFilterNode.getCPtr(x), x);
  }

  private WrappedServiceSubscriptionFilterNode doRemove(int index) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doRemove(swigCPtr, this, index);
    return (cPtr == 0) ? null : new WrappedServiceSubscriptionFilterNode(cPtr, true);
  }

  private WrappedServiceSubscriptionFilterNode doGet(int index) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doGet(swigCPtr, this, index);
    return (cPtr == 0) ? null : new WrappedServiceSubscriptionFilterNode(cPtr, true);
  }

  private WrappedServiceSubscriptionFilterNode doSet(int index, WrappedServiceSubscriptionFilterNode val) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doSet(swigCPtr, this, index, WrappedServiceSubscriptionFilterNode.getCPtr(val), val);
    return (cPtr == 0) ? null : new WrappedServiceSubscriptionFilterNode(cPtr, true);
  }

  private void doRemoveRange(int fromIndex, int toIndex) {
    RobotRaconteurJavaJNI.vectorptr_wrappedservicesubscriptionnode_doRemoveRange(swigCPtr, this, fromIndex, toIndex);
  }

}
